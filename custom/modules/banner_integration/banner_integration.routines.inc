<?php
/**
 * @file
 * This file contains functions which are meant to be called from nightly routines.
 * 
 * Please see the sample_rotune.php file for an example of how to set up a nightly
 * routine, using the functions in this file.
 * 
 *
 */


// Make sure we are connected to our Banner oracle instance:
banner_integration_connect_if_not_connected();

// We want to die if there are Oracle or MySQL errors
// You can set these back to FALSE right after including this file, if you wish.
$GLOBALS["fp_oracleapi_die_oracle_errors"] = TRUE;
$GLOBALS["fp_die_mysql_errors"] = TRUE;




/**
 * This function will pull in the transfer institutions from Banner, into
 * FlightPath's transfer_institutions table.
 *
 */
function banner_integration_routines_import_transfer_institutions() {    

  watchdog("banner_integration", "Starting routine ...import_transfer_institutions");  
  $c = 0;
  // Make sure Banner is still available by grabbing the current term.  This
  // act will kill the script if it is not available.
  $cur_term_details = banner_integration_get_current_term_details();    
   
  db_query("TRUNCATE transfer_institutions");

  $res = oracleapi_query("
        select stvsbgi_code, stvsbgi_desc
        from stvsbgi      
        ") or die(oracleapi_error());
  
  while ($cur = oracleapi_fetch_array($res)) {
    $institution_id = $cur["STVSBGI_CODE"];
    $institution_name = $cur["STVSBGI_DESC"];
    
    // Let's figure out the state, if it has one.
    $res2 = oracleapi_query("SELECT sobsbgi_stat_code
                          FROM sobsbgi
                          WHERE sobsbgi_sbgi_code = '?' ", $institution_id);
    $cur2 = oracleapi_fetch_array($res2);    
    $state = trim($cur2["SOBSBGI_STAT_CODE"]);
    
    // Okay, let's insert into MySQL.
    $res2 = db_query("INSERT INTO transfer_institutions (institution_id, name, state)
                          VALUES ('?', '?', '?')", $institution_id, $institution_name, $state);
    
    $c++;
  }   
  
  
  // All done!
  watchdog("banner_integration", "Finished routine ...import_transfer_institutions.
                                  @count institutions added to transfer_institutions table.",
                                  array("@count" => $c));
  
  
}




/**
 * This routine will populate the transfer_eqv_global table which comes with the
 * transfer_search module.
 * 
 * DO NOT RUN if you don't have the transfer_search module installed!  Otherwise
 * the query will fail.
 *
 */
function banner_integration_routines_import_transfer_eqv_global() {
  
  watchdog("transfer_search", "Starting routine ...import_transfer_eqv_global");  
  
  $db = get_global_database_handler();
  
  // Begin by truncating the transfer_eqv_table.
  db_query("TRUNCATE transfer_eqv_global ");

  // Now, we perform our gigantic oracle query to get the Banner data.
  $res = oracleapi_query("
  
               SELECT 
                shbtatc_sbgi_code as institution_id,
                shrtatc_tlvl_code as career_level,
                shbtatc_term_code_eff_trns as term_effective,
                shbtatc_subj_code_trns as transfer_subject_id,
                shbtatc_crse_numb_trns as transfer_course_num,
                shrtatc_subj_code_inst as local_subject_id,
                shrtatc_crse_numb_inst as local_course_num,
                replace(shbtatc_trns_title,',',' ') as transfer_course_title,
                replace(shrtatc_inst_title,',',' ') as local_course_title,
                shrtatc_inst_credits_used as local_hours,
                shrtatc_seqno as seq_num,
                max_seq,
                shrtatc_connector,
                shrtatc_inst_lparen_conn,
                shrtatc_inst_rparen
              FROM shbtatc a, shrtatc b
              , 
              (select shrtatc_sbgi_code, shrtatc_subj_code_trns, shrtatc_crse_numb_trns, max(shrtatc_seqno) max_seq,  max(shrtatc_term_code_eff_trns) max_term
                from shrtatc
                 group by shrtatc_sbgi_code, shrtatc_subj_code_trns, shrtatc_crse_numb_trns) c
                
              WHERE shbtatc_sbgi_code = b.shrtatc_sbgi_code
                and shbtatc_subj_code_trns = b.shrtatc_subj_code_trns
                and shbtatc_crse_numb_trns = b.shrtatc_crse_numb_trns
                and shbtatc_term_code_eff_trns = b.shrtatc_term_code_eff_trns
                and (shbtatc_trns_high_hrs is not null or shbtatc_trns_high_hrs > 0)
                and shbtatc_term_code_eff_trns = (
                                                    select max(shbtatc_term_code_eff_trns) from shbtatc
                                                      where shbtatc_sbgi_code = a.shbtatc_sbgi_code
                                                        and shbtatc_subj_code_trns = a.shbtatc_subj_code_trns
                                                        and shbtatc_crse_numb_trns = a.shbtatc_crse_numb_trns
                                                )
                and b.shrtatc_sbgi_code = c.shrtatc_sbgi_code
                and b.shrtatc_subj_code_trns = c.shrtatc_subj_code_trns
                and b.shrtatc_crse_numb_trns = c.shrtatc_crse_numb_trns
                and b.shrtatc_term_code_eff_trns = max_term
                and (shbtatc_tast_code != 'I' or shbtatc_tast_code is null)
                and b.shrtatc_tlvl_code in ('UG', 'PR')
                ORDER BY '1', '3', '4', '5', '11'
              
                                      "); 
  
  while ($cur = oracleapi_fetch_array($res)) {
    // Make lowercase
    $cur = array_change_key_case($cur);
    // Extract for convenience
    extract($cur, 3, "db");
    
    
    // In development:  only do X number...
    // if ($c > 50) continue;
    
    // Get local and transfer course id and hours from our transfer_courses table.
    $transfer_course_id = "";
    $transfer_hours = 0;
    $local_course_id = 0;
    
    // Get local_course_id first
    $local_course_id = $db->get_course_id($db_local_subject_id, $db_local_course_num);
    
    // Get transfer data...
    $res2 = db_query("SELECT * FROM transfer_courses
                      WHERE institution_id = '?'
                      AND subject_id = '?'
                      AND course_num = '?' ", $db_institution_id, $db_transfer_subject_id, $db_transfer_course_num);
    $cur2 = db_fetch_array($res2);
    $transfer_course_id = $cur2["transfer_course_id"];
    $transfer_hours = $cur2["min_hours"]*1;
    
    // If the $transfer_hours is zero, try the max hours...
    if ($transfer_hours < 1) {
      $transfer_hours = $cur2["max_hours"]*1;
    }
    
    // If $transfer_hours is STILL zero, there might be a problem.  Set it to the
    // number of local hours from the eqv.
    if ($transfer_hours < 1) {
      $transfer_hours = $db_local_hours * 1;
    }
    
    
    
    // Now, we can insert into our table...
    db_query("  
      INSERT into transfer_eqv_global
        (institution_id, transfer_subject_id, transfer_course_num, transfer_course_id, transfer_hours,
         local_subject_id, local_course_num, local_course_id, transfer_course_title, local_course_title,
         local_hours, seq_num, max_seq)
       VALUES 
        ('?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?') ",
        $db_institution_id, $db_transfer_subject_id, 
        $db_transfer_course_num, $transfer_course_id, $transfer_hours,
         $db_local_subject_id, $db_local_course_num, $local_course_id, $db_transfer_course_title, 
         $db_local_course_title,
         $db_local_hours, $db_seq_num, $db_max_seq);
          
    
    $c++;
  }
  
  
  // All done!
  watchdog("banner_integration", "Finished routine ...import_transfer_eqv_global.
                                  @count rows added to transfer_rqv_global table",
                                  array("@count" => $c));  
  
  
}






/** ORIGINAL - ADDED BACK ON 20 SEPTEMBER 2017
 * This routine populate's FP's courses and draft_courses table with course data from Banner.
 * 
 * We will look at Banner's SCBCRSE table to populate the "local" courses (meaning,
 * non-transfer) courses in FlightPath.  We will use settings set on the admin config form
 * for this module to decide what we do with the data when we get it.
 *
 */
function banner_integration_routines_import_local_courses() {

  watchdog("banner_integration", "Starting routine ...import_local_courses");  
  // Make sure Banner is still available by grabbing the current term.  This
  // act will kill the script if it is not available.
  
  $cur_term_details = banner_integration_get_current_term_details();    
  
  
  $db = get_global_database_handler();
  
  // Get our settings from the admin config form:
  $bool_skip_if_exists = FALSE;
  $bool_update_prod_and_draft = FALSE;
  $temp_course = new Course();
  
  $settings = variable_get("banner_integration_routines_import_local_courses_settings", array());
  if ($settings["skip_if_exists"] == "skip_if_exists") {
    // We should skip any courses which already exist in FP's db that we find, which match
    // the same subject_id and course_num.
    $bool_skip_if_exists = TRUE;
  }
  if ($settings["update_prod_and_draft"] == "update_prod_and_draft") {
    // We should update both the draft_courses table, as well as courses table, when 
    // any modifications are to be make.
    $bool_update_prod_and_draft = TRUE;
  }
   
  

  // Okay, let's begin by searching through the SCBCRSE table for courses
  $res = oracleapi_query("SELECT * FROM scbcrse");
  while ($cur = oracleapi_fetch_array($res)) {
    // Pull out all of the data we will need.
    $subject_id = $cur["SCBCRSE_SUBJ_CODE"];
    $course_num = $cur["SCBCRSE_CRSE_NUMB"];
    $eff_term = $cur["SCBCRSE_EFF_TERM"];
    // Get the catalog year based on this term
	//--CODE-ADD-START
	//--Catalog year function call for truncating term when importing courses
	//--CODE-ADD-END
    $catalog_year = banner_integration_get_catalog_year_for_catalog_term($eff_term);
    
    // Fix the title (make it's case conform to what we're used to)
    $title = $temp_course->fix_title($cur["SCBCRSE_TITLE"]);
    
    $min_hours = $cur["SCBCRSE_CREDIT_HR_LOW"];
    $max_hours = $cur["SCBCRSE_CREDIT_HR_HIGH"];
    if ($max_hours == "" || !is_numeric($max_hours)) {
      $max_hours = $min_hours;
    }
    
    // A rough guess:
    $repeat_hours = $max_hours;
    
    $bool_insert_course = FALSE;
    
    // Okay, first things first-- does this course already exist in our database?
    // We will check in draft.
    $course_id = $db->get_course_id($subject_id, $course_num, "", TRUE);
    
    if ($course_id) {
      // Yes, it was found.
      
      // Should we skip it?
      if ($bool_skip_if_exists) {
        continue;
      }
      
      // We are here, so we should figure out if we are adding a new row for this catalog_year,
      // or are we updating an existing field?  To answer this, we will test again, but specify
      // the catalog year this time.
      $test_course_id = $db->get_course_id($subject_id, $course_num, $catalog_year, TRUE);
      
      if ($test_course_id) {
        // Yes, the course was found for this catalog year.   Let's just do an update, so as
        // not to override other settings, like the delete_flag, description, etc.
        db_query("UPDATE draft_courses
                  SET title = '?',
                      min_hours = '?',
                      max_hours = '?',
                      repeat_hours = '?',
                      data_entry_comment = '?'
                  WHERE
                      subject_id = '?'
                  AND course_num = '?'
                  AND catalog_year = '?'
                  ", $title, $min_hours, $max_hours, $repeat_hours,
                     "IMPORTED ~ Imported from banner_integration.routines.inc routine ...import_local_courses
                     on " . format_date(time()),
                     $subject_id, $course_num, $catalog_year);

        // Also do courses if requested
        if ($bool_update_prod_and_draft) {
          db_query("UPDATE courses
                    SET title = '?',
                        min_hours = '?',
                        max_hours = '?',
                        repeat_hours = '?',
                        data_entry_comment = '?'
                    WHERE
                        subject_id = '?'
                    AND course_num = '?'
                    AND catalog_year = '?'
                    ", $title, $min_hours, $max_hours, $repeat_hours,
                       "IMPORTED ~ Imported from banner_integration.routines.inc routine ...import_local_courses
                       on " . format_date(time()),
                       $subject_id, $course_num, $catalog_year);
          
        }
          
                
      }
      else {
        // No, the course was not found for this catalog year, so we can just insert it normally
        // under the course_id we found, for this catalog year.
        $bool_insert_course = TRUE;
      }
      
      
    }
    else {
      // No, the course was not found in the database at all.  Request a new course_id
      // and insert the course.
      $bool_insert_course = TRUE;
      $course_id = $db->request_new_course_id();
      if (!$course_id) {
        die("Error - Was not able to request a new course id from the database.");
      }
    }
    
    
    if ($bool_insert_course) {
      db_query("INSERT INTO draft_courses
                  (course_id, subject_id, course_num, catalog_year, title,
                   min_hours, max_hours, repeat_hours, data_entry_comment)
                VALUES 
                   ('?', '?', '?', '?', '?', '?', '?', '?', '?')
                ", $course_id, $subject_id, $course_num, $catalog_year, $title,
                    $min_hours, $max_hours, $repeat_hours,
                    "IMPORTED ~ Imported from banner_integration.routines.inc routine ...import_local_courses
                     on " . format_date(time()) );
      // Also do courses if requested
      if ($bool_update_prod_and_draft) {
        db_query("INSERT INTO courses
                    (course_id, subject_id, course_num, catalog_year, title,
                     min_hours, max_hours, repeat_hours, data_entry_comment)
                  VALUES 
                     ('?', '?', '?', '?', '?', '?', '?', '?', '?')
                  ", $course_id, $subject_id, $course_num, $catalog_year, $title,
                      $min_hours, $max_hours, $repeat_hours,
                      "IMPORTED ~ Imported from banner_integration.routines.inc routine ...import_local_courses
                       on " . format_date(time()) );
        
      }
                    
                    
    }
     
    
    
  } 
  
   
  // All done!
  watchdog("banner_integration", "Finished routine ...import_local_courses");  
  
  
}

/** FOR BSCCHEM ONLY
 * This routine populate's FP's courses and draft_courses table with course data from Banner.
 * 
 * We will look at Banner's SCBCRSE table to populate the "local" courses (meaning,
 * non-transfer) courses in FlightPath.  We will use settings set on the admin config form
 * for this module to decide what we do with the data when we get it.
 *
 */
function banner_integration_routines_import_local_courses_bscchem() {

  watchdog("banner_integration", "Starting routine ...import_local_courses");  
  // Make sure Banner is still available by grabbing the current term.  This
  // act will kill the script if it is not available.
  
  //banner_integration_connect_if_not_connected();
  //$cur_term_details = banner_integration_get_current_term_details();    
  
  
  $db = get_global_database_handler();
  
  // Get our settings from the admin config form:
  $bool_skip_if_exists = FALSE;
  $bool_update_prod_and_draft = FALSE;
  $temp_course = new Course();
  
  $settings = variable_get("banner_integration_routines_import_local_courses_settings", array());
  if ($settings["skip_if_exists"] == "skip_if_exists") {
    // We should skip any courses which already exist in FP's db that we find, which match
    // the same subject_id and course_num.
    $bool_skip_if_exists = TRUE;
  }
  if ($settings["update_prod_and_draft"] == "update_prod_and_draft") {
    // We should update both the draft_courses table, as well as courses table, when 
    // any modifications are to be make.
    $bool_update_prod_and_draft = TRUE;
  }
   
  

  // Okay, let's begin by searching through the SCBCRSE table for courses
// $res = oracleapi_query("SELECT * FROM scbcrse");
  $res = oracleapi_query("SELECT * FROM JPAZHAI.BSCCHEM");
  while ($cur = oracleapi_fetch_array($res)) {
    // Pull out all of the data we will need.
    $subject_id = $cur["SCBCRSE_SUBJ_CODE"];
    $course_num = $cur["SCBCRSE_CRSE_NUMB"];
    //CODE-DELETE-START -201708XX - removed to pull courses for a specific term below
    //$eff_term = $cur["SCBCRSE_EFF_TERM"];
    // Get the catalog year based on this term
    // $catalog_year = banner_integration_get_catalog_year_for_catalog_term($eff_term);
    //CODE-DELETE-END
	
   //CODE-ADD-START -2017XXXX Catalog year function call for truncating term when importing courses replaced with year
    $catalog_year = 2011;
    //--CODE-ADD-END
    // Fix the title (make it's case conform to what we're used to)
    $title = $temp_course->fix_title($cur["SCBCRSE_TITLE"]);
    
    $min_hours = $cur["SCBCRSE_CREDIT_HR_LOW"];
    $max_hours = $cur["SCBCRSE_CREDIT_HR_HIGH"];
    if ($max_hours == "" || !is_numeric($max_hours)) {
      $max_hours = $min_hours;
    }
    
    // A rough guess:
    $repeat_hours = $max_hours;
    
    $bool_insert_course = FALSE;
    
    // Okay, first things first-- does this course already exist in our database?
    // We will check in draft.
    $course_id = $db->get_course_id($subject_id, $course_num, "", TRUE);
    
    if ($course_id) {
      // Yes, it was found.
      
      // Should we skip it?
      if ($bool_skip_if_exists) {
        continue;
      }
      
      // We are here, so we should figure out if we are adding a new row for this catalog_year,
      // or are we updating an existing field?  To answer this, we will test again, but specify
      // the catalog year this time.
      $test_course_id = $db->get_course_id($subject_id, $course_num, $catalog_year, TRUE);
      
      if ($test_course_id) {
        // Yes, the course was found for this catalog year.   Let's just do an update, so as
        // not to override other settings, like the delete_flag, description, etc.
        db_query("UPDATE draft_courses
                  SET title = '?',
                      min_hours = '?',
                      max_hours = '?',
                      repeat_hours = '?',
                      data_entry_comment = '?'
                  WHERE
                      subject_id = '?'
                  AND course_num = '?'
                  AND catalog_year = '?'
                  ", $title, $min_hours, $max_hours, $repeat_hours,
                     "IMPORTED ~ Imported from banner_integration.routines.inc routine ...import_local_courses
                     on " . format_date(time()),
                     $subject_id, $course_num, $catalog_year);

        // Also do courses if requested
        if ($bool_update_prod_and_draft) {
          db_query("UPDATE courses
                    SET title = '?',
                        min_hours = '?',
                        max_hours = '?',
                        repeat_hours = '?',
                        data_entry_comment = '?'
                    WHERE
                        subject_id = '?'
                    AND course_num = '?'
                    AND catalog_year = '?'
                    ", $title, $min_hours, $max_hours, $repeat_hours,
                       "IMPORTED ~ Imported from banner_integration.routines.inc routine ...import_local_courses
                       on " . format_date(time()),
                       $subject_id, $course_num, $catalog_year);
          
        }
          
                
      }
      else {
        // No, the course was not found for this catalog year, so we can just insert it normally
        // under the course_id we found, for this catalog year.
        $bool_insert_course = TRUE;
      }
      
      
    }
    else {
      // No, the course was not found in the database at all.  Request a new course_id
      // and insert the course.
      $bool_insert_course = TRUE;
      $course_id = $db->request_new_course_id();
      if (!$course_id) {
        die("Error - Was not able to request a new course id from the database.");
      }
    }
    
    
    if ($bool_insert_course) {
      db_query("INSERT INTO draft_courses
                  (course_id, subject_id, course_num, catalog_year, title,
                   min_hours, max_hours, repeat_hours, data_entry_comment)
                VALUES 
                   ('?', '?', '?', '?', '?', '?', '?', '?', '?')
                ", $course_id, $subject_id, $course_num, $catalog_year, $title,
                    $min_hours, $max_hours, $repeat_hours,
                    "IMPORTED ~ Imported from banner_integration.routines.inc routine ...import_local_courses
                     on " . format_date(time()) );
      // Also do courses if requested
      if ($bool_update_prod_and_draft) {
        db_query("INSERT INTO courses
                    (course_id, subject_id, course_num, catalog_year, title,
                     min_hours, max_hours, repeat_hours, data_entry_comment)
                  VALUES 
                     ('?', '?', '?', '?', '?', '?', '?', '?', '?')
                  ", $course_id, $subject_id, $course_num, $catalog_year, $title,
                      $min_hours, $max_hours, $repeat_hours,
                      "IMPORTED ~ Imported from banner_integration.routines.inc routine ...import_local_courses
                       on " . format_date(time()) );
        
      }
                    
                    
    }
     
    
    
  } 
  
   
  // All done!
  watchdog("banner_integration", "Finished routine ...import_local_courses");  
  
  
}





/**
 * Populate the flightpath advisor_student table with data from Banner's sgradvr table.
 *
 */
function banner_integration_routines_import_advisor_student() {
  
  watchdog("banner_integration", "Starting routine ...import_advisor_student");  

  // Make sure Banner is still available by grabbing the current term.  This
  // act will kill the script if it is not available.
  $cur_term_details = banner_integration_get_current_term_details();    
  
  
  
  db_query("TRUNCATE advisor_student");
  
  $c = 0;
  
  // We want to begin by finding all of the advisors' pidms (uniquely) in the sgradvr table.  
  $res = oracleapi_query("select unique sgradvr_advr_pidm FROM sgradvr");
  while ($cur = oracleapi_fetch_array($res)) {
    $advisor_pidm = $cur["SGRADVR_ADVR_PIDM"];
    
    // Okay, now for every advisor, find the *most recent* students assigned to them.
    // For this, we will have to look at the term_code_eff value, and select the max available
    // for each student (because a student can change advisors)
    $res2 = oracleapi_query("
                              SELECT sgradvr_pidm
                              FROM sgradvr a
                              WHERE sgradvr_advr_pidm = '?'
                                AND sgradvr_term_code_eff = (
                                                            SELECT max(sgradvr_term_code_eff) 
                                                            FROM sgradvr 
                                                            WHERE sgradvr_pidm = a.sgradvr_pidm
                                                          )    
              ", $advisor_pidm);
    while ($cur2 = oracleapi_fetch_array($res2)) {
      // Okay, we are almost done!  Before we can insert into our mysql table,
      // we need to convert the PIDMs into cwids.
      $student_pidm = $cur2["SGRADVR_PIDM"];
      
      $advisor_cwid = banner_integration_get_cwid_for_pidm($advisor_pidm);
      $student_cwid = banner_integration_get_cwid_for_pidm($student_pidm);
      
      // Okay, now insert into our mysql table!
      // We will use INSERT IGNORE because we don't care about duplicate errors, should
      // the same advisor/student combo get added more than once.
      db_query("INSERT IGNORE INTO advisor_student (faculty_id, student_id)
                VALUES ('?', '?') ", $advisor_cwid, $student_cwid);
      
      $c++;
    }
        
  }

  // All done! 
  watchdog("banner_integration", "Finished routine ...import_advisor_student.
                                  @count advisor/student records added (duplicates ignored)
                                  ", array("@count" => $c));    
  
}




/**
 * Simple function to populate the scratch_goremal table.
 * 
 * This function is used by other routines, so I don't have to copy code.
 * 
 *
 */
function banner_integration_routines_populate_scratch_goremal() {
    
    db_query("TRUNCATE banner_integration_scratch_goremal");  
    $res = oracleapi_query("SELECT goremal_pidm, goremal_email_address, goremal_emal_code 
                            FROM goremal 
                            WHERE (goremal_emal_code = 'ST' OR goremal_emal_code = 'HR')
  		                      AND goremal_status_ind = 'A' ");
    while ($cur = oracleapi_fetch_array($res)) {
      db_query("INSERT INTO banner_integration_scratch_goremal
                  (goremal_pidm, goremal_email_address, goremal_emal_code)
                VALUES ('?', '?', '?')
                ", $cur["GOREMAL_PIDM"], $cur["GOREMAL_EMAIL_ADDRESS"], $cur["GOREMAL_EMAL_CODE"]);
    }    
}




/**
 * Like the ...import_students function, this will attempt to import our faculty/staff users.
 * 
 * In FlightPath, we actually don't make any distinction between faculty and staff, and for
 * simplicity just refer to everyone as "faculty."
 * 
 * Please not that this may be a fairly long routine.  It will make use of temporary tables,
 * scratch tables, etc.  Depending on the number of faculty/staff, it could take between 10 - 30
 * minutes to run.
 * 
 * @param $bool_print_debug_messages
 *    If set to TRUE, debug messages will print on the command line
 * @param $bool_populate_scratch_goremal
 *    If set to TRUE, the scratch table goremal will be cleared, then populated when the script runs.
 *    You would set this to FALSE if you know that table is already populated by another routine.
 * @param $bool_truncate_scratch_goremal
 *    - If set to TRUE, the scratch table goremal will be cleared when this routine finishes.
 *    - You would set this to FALSE if you know that other routines (like the import_faculty_staff routine)
 *      also need this table.
 */
function banner_integration_routines_import_faculty_staff($bool_print_debug_messages = TRUE, $bool_populate_scratch_goremal = TRUE, $bool_truncate_scratch_goremal = TRUE) {

  watchdog("banner_integration", "Starting routine ...import_faculty_staff");  

  // Make sure Banner is still available by grabbing the current term.  This
  // act will kill the script if it is not available.
  $cur_term_details = banner_integration_get_current_term_details();    
    
  
  
	$start_time = time();
	
  if ($bool_print_debug_messages) {
    print "\n -- Beginning import_faculty_staff routine - $start_time \n";
  }

  
  // We know we will need the goremal data later.
  if ($bool_populate_scratch_goremal) {
   
    if ($bool_print_debug_messages) {
      print "\n -- Reading from Banner's goremal table and writing to scratch table... - " . time() . " \n";
    }
    
    watchdog("banner_integration", "routine ...import_faculty_staff: Reading from banner's goremal
                                    table and writing to scratch table.");  
      
    banner_integration_routines_populate_scratch_goremal();
    
  }  
  
  
 if ($bool_print_debug_messages) {
    print "\n -- Assembling our new faculty data in a temporary table and users table - " . time() . " \n";
  }
  
  watchdog("banner_integration", "routine ...import_faculty_staff: Assembling our new faculty
                                  data in a temporary table, and also writing to the users table.");  
      
  
  
  // Create our temp table
  db_query("CREATE TEMPORARY TABLE `TEMP_faculty` LIKE `faculty`");
  
  
  // Okay, now query the ap_employment_verification view for our data.
  $res = oracleapi_query("SELECT * FROM baninst1.ap_employment_verification
                          WHERE empl_status = 'A' ");
  while ($cur = oracleapi_fetch_array($res)) {
    $cur = array_change_key_case($cur);

    // Get email and username:
    $res2 = db_query("SELECT goremal_email_address 
                             FROM banner_integration_scratch_goremal 
                             WHERE goremal_pidm = '?'
                             AND goremal_emal_code = 'HR'
                             ", $cur["pidm_key"]);
    $cur2 = db_fetch_array($res2);
    $email = $cur2["goremal_email_address"];
    // TODO:  Have this username discovery configurable.
    $temp = explode("@", $email);
    
    $username = trim($temp[0]);        

  
    // Okay, we can now insert into our TEMP_faculty table
    db_query("INSERT INTO TEMP_faculty
              (cwid, department_code, department)
              VALUES ('?', '?', '?')
              ", $cur["id_number"], $cur["orgn_code_home"], $cur["orgn_code_home_desc"]);
    
    // Okay, we also want to see if this user is already in our users table.  If they are,
    // we will UPDATE the table.  If they are not, we will INSERT.
    $test_user_id = db_get_user_id_from_cwid($cur["id_number"], "faculty");
    if ($test_user_id) {
      // Yes, it's there, so let's update.
      db_query("UPDATE users
                SET user_name = '?',
                    email = '?',
                    cwid = '?',
                    f_name = '?',
                    l_name = '?'
                WHERE user_id = '?'
                ", $username, $email, $cur["id_number"], $cur["first_name"], $cur["last_name"], $test_user_id);
                    
    }
    else {
      // The user was NOT already there, so let's add them in.
      db_query("INSERT INTO users (user_name, email, cwid, f_name, l_name, is_faculty)
                VALUES ('?', '?', '?', '?', '?', '1')
                ", $username, $email, $cur["id_number"], $cur["first_name"], $cur["last_name"]);
    }
     
    
    
       
  }
  
  if ($bool_print_debug_messages) {
    print "\n -- Finished writing to temp table and users - " . time() . " \n";
  }
  
  watchdog("banner_integration", "routine ...import_faculty_staff: Finished writing to
                                  temp table and users.");  
  
  
  //////////  Almost done!
  if ($bool_print_debug_messages) {
    print "\n -- Copying temp table to real table. \n";
  }

  watchdog("banner_integration", "routine ...import_faculty_staff: Copying from temp table to real table.");  
    
  
  // Copy our temp table to our real table.
  db_query("TRUNCATE faculty");
  db_query("INSERT faculty SELECT * FROM `TEMP_faculty` ");
  db_query("DROP TABLE `TEMP_faculty` ");  
  
  // Clean our scratch table
  if ($bool_truncate_scratch_goremal) {
    db_query("TRUNCATE banner_integration_scratch_goremal");
  }

  
  $end_time = time();
   
  // And we're done!
  if ($bool_print_debug_messages) {
    print "\n -- Finished with import_faculty_staff! Done in ". ($end_time - $start_time) . " seconds total.\n";
  }

  
  watchdog("banner_integration", "Finished routine ...import_faculty_staff in @seconds seconds", array("@seconds" => ($end_time - $start_time)));  
  
 
   
}



/**
 * This function will attempt to import all of the students in Banner
 * 
 * This is a VERY intensive routine, and it should only be run at off-peak times.  Data will
 * first be loaded into a temporary table, then copied to the normal tables all at once, to minimize
 * downtime.  Depending on the number of students in Banner, this could take 30-60 minutes or longer
 * to run.
 *
 * PLEASE NOTE:  The way that catalog years are deciphered from catalog terms is through a hook
 *               system.  They are by default set to the way The University of Louisiana at Monroe
 *               has them.  You will need to implement your own hook (in your own module) if you
 *               would like to override this behavior!  See README.txt for more information.
 *  
 * @param $bool_print_debug_messages
 *    If set to TRUE, debug messages will print on the command line
 * @param $bool_populate_scratch_goremal
 *    If set to TRUE, the scratch table goremal will be cleared, then populated when the script runs.
 *    You would set this to FALSE if you know that table is already populated by another routine.
 * @param $bool_truncate_scratch_goremal
 *    - If set to TRUE, the scratch table goremal will be cleared when this routine finishes.
 *    - You would set this to FALSE if you know that other routines (like the import_faculty_staff routine)
 *      also need this table.
 */
function banner_integration_routines_import_students($bool_print_debug_messages = TRUE, $bool_populate_scratch_goremal = TRUE, $bool_truncate_scratch_goremal = TRUE) {
  
  
  
  watchdog("banner_integration", "Starting routine ...import_students");  

  // Make sure Banner is still available by grabbing the current term.  This
  // act will kill the script if it is not available.
  $cur_term_details = banner_integration_get_current_term_details();    
    
  
  
	$start_time = time();
	
  if ($bool_print_debug_messages) {
    print "\n -- Beginning import_students routine - $start_time \n";
  }
  
  // This is going to have to be a 2-pass import routine
  // First, we will import the Banner view sgvstd1, for the current and next term,
  // into a local table, so we can read from it more quickly when it comes time to build up
  // our users/students table.
  
  // Let's begin by getting our current term, as well as future terms up to 120 days into the future.
  // This way, a student who may not be registered in this term, might be in the next, and we would
  // want them accessible in FlightPath.

  // Query for terms:
  $res = oracleapi_query("
          select stvterm_desc as cur_desc, nvl(stvterm_code,(select min(stvterm_code)
                             from stvterm
                            where stvterm_start_date > trunc(sysdate))) as term_code
              from stvterm
              where stvterm_end_date >= trunc(sysdate)
              and to_char(stvterm_start_date,'J') <= (to_char(sysdate,'J') + 120) 
            ");
  while ($cur = oracleapi_fetch_array($res)) {
    $terms[$cur["TERM_CODE"]] = $cur["TERM_CODE"];
  }
   
  // Make sure the current term is in our $terms array too!
  $cur_term_details = banner_integration_get_current_term_details();
  $cur_term = $cur_term_details["term_id"];
  
  $terms[$cur_term] = $cur_term;
 
  // Sort the terms array so that the biggest (most-future) is first.
  arsort($terms);
  
  //////////////////////////////////////////////
  /////  Step One:  Read into our scratch tables
  //////////////////////////////////////////////

  if ($bool_populate_scratch_goremal) {
   
    if ($bool_print_debug_messages) {
      print "\n -- Reading from Banner's goremal table and writing to scratch table... - " . time() . " \n";
    }
    
    watchdog("banner_integration", "routine ...import_students: Reading from banner's goremal
                                    table and writing to scratch table.");  
  
    
    banner_integration_routines_populate_scratch_goremal();
    
  }
    
  // Okay, now that we have the terms we are interested in, we will read from Banner's
  // sgvstd1 view for those terms, and write the fields we need to our scratch table.
  if ($bool_print_debug_messages) {
    print "\n -- Reading from Banner's sgvstd1 view and writing to scratch table... - " . time() . " \n";
  }
 
  watchdog("banner_integration", "routine ...import_students: Reading from banner's sgvstd1 view
                                  and writing to scratch table.");  
  
    
  // Clear out our scratch table.
  db_query("TRUNCATE banner_integration_scratch_sgvstd1");  
  
  // Okay, start our oracle query
  // Added--
  //STST_CODE IN ('AS','RA','XP','XT') 
 //                    AND
// --condition to reduce # of rows fetched from Oracle, by pulling only currently active students
  //-CODE-DELETE-START-20171119 replacing with new query which does not hit the view but fetches directly from the table.
  /*
  
  $res = oracleapi_query("
                      SELECT  pidm_key, term_code_key, term_code_admit, stst_code, enrolled_ind, registered_ind, id, 
                              last_name, first_name, clas_code, majr_code,
                              majr_code_concentration_1, levl_gpa_hours_earned1, levl_gpa1
                      FROM baninst1.sgvstd1
                      WHERE STST_CODE IN ('AS','RA','XP','XT') 
                      AND term_code_key IN ('" . join("','", $terms) . "')
                        ");
  //CODE-DELETE-END
  */
  //CODE-ADD-START-20171119 new query
  $res = oracleapi_query("
SELECT 		PIDM_KEY, 
            TERM_CODE_KEY,
            TERM_CODE_ADMIT,
            STST_CODE,
            ENROLLED_IND,
            REGISTERED_IND,
            ID,
            LAST_NAME,
            FIRST_NAME,
            CLAS_CODE,
            MAJR_CODE,
            MAJR_CODE_CONCENTRATION_1,
            TO_NUMBER(SUBSTR(F_SPLIT_FIELDS(AA, 1), 1, 23), '99999999999999D999999999') LEVL_GPA_HOURS_EARNED1,
            TO_NUMBER(SUBSTR(F_SPLIT_FIELDS(AA, 5), 1, 23), '99999999999999D999999999') LEVL_GPA1
FROM (
SELECT  A.SGBSTDN_PIDM 																						PIDM_KEY,
			STVTERM_CODE 																						TERM_CODE_KEY,
			A.SGBSTDN_TERM_CODE_ADMIT																			TERM_CODE_ADMIT,
			A.SGBSTDN_STST_CODE																					STST_CODE,
			SUBSTR(F_ENROLLED_THIS_TERM(A.SGBSTDN_PIDM, STVTERM_CODE), 1, 1)									ENROLLED_IND,
			SUBSTR(F_REGISTERED_THIS_TERM(A.SGBSTDN_PIDM, STVTERM_CODE), 1, 1)									REGISTERED_IND,
			SUBSTR(GOKSELS.F_NAME_BIO_VALUE(A.SGBSTDN_PIDM, 'ID'), 1, 9)										ID,
			SUBSTR(GOKSELS.F_NAME_BIO_VALUE(A.SGBSTDN_PIDM, 'LAST'), 1, 60)										LAST_NAME,
			SUBSTR(GOKSELS.F_NAME_BIO_VALUE(A.SGBSTDN_PIDM, 'FIRST'), 1, 60)									FIRST_NAME,
			SUBSTR(F_CLASS_CALC_FNC(A.SGBSTDN_PIDM, A.SGBSTDN_LEVL_CODE, STVTERM_CODE), 1, 2)					CLAS_CODE,              
			A.SGBSTDN_MAJR_CODE_1																				MAJR_CODE,
	A.SGBSTDN_PROGRAM_1																					MAJR_CODE_CONCENTRATION_1,
			F_CONCAT_AS_OF_CUM_GPA(A.SGBSTDN_PIDM, STVTERM_CODE, A.SGBSTDN_LEVL_CODE, 'O', A.SGBSTDN_CAMP_CODE)	AA
FROM		SGBSTDN A, STVTERM
WHERE		A.SGBSTDN_STST_CODE IN ('AS','AW','CW','DA','DW','OW','PA','RA','SP','WN','XP','XT')
AND         A.SGBSTDN_TERM_CODE_EFF = SB_LEARNER.F_QUERY_CURRENT(A.SGBSTDN_PIDM, STVTERM_CODE)
  AND term_code_key IN ('" . join("','", $terms) . "')
                        ");
  //CODE-ADD-END
  
  
  
  while ($cur = oracleapi_fetch_array($res)) {
    db_query("INSERT INTO banner_integration_scratch_sgvstd1
                  (pidm_key, term_code_key, term_code_admit, enrolled_ind, registered_ind, stst_code, `id`, 
                   last_name, first_name, clas_code, majr_code,
                   majr_code_concentration_1, levl_gpa_hours_earned1, levl_gpa1)
              VALUES ('?', '?', '?', '?', '?', '?', '?',
                      '?', '?', '?', '?',
                      '?', '?', '?')
              ", $cur["PIDM_KEY"], $cur["TERM_CODE_KEY"], $cur["TERM_CODE_ADMIT"], $cur["ENROLLED_IND"], $cur["REGISTERED_IND"], $cur["STST_CODE"], $cur["ID"],
                 $cur["LAST_NAME"], $cur["FIRST_NAME"], $cur["CLAS_CODE"], $cur["MAJR_CODE"],
                 $cur["MAJR_CODE_CONCENTRATION_1"], $cur["LEVL_GPA_HOURS_EARNED1"], $cur["LEVL_GPA1"]);
  }
  
  if ($bool_print_debug_messages) {
    print "\n -- Finished writing to scratch table... \n";
  }

  watchdog("banner_integration", "routine ...import_students: Finished writing to
                                  scratch table.");  
  
    
  /////////////////////////////////
  ////   Step Two:  Copy from scratch table into a temporary table version of our students table.
  ////              Also, go ahead and add to our users table, if not already there.
  /////////////////////////////////

  if ($bool_print_debug_messages) {
    print "\n -- Assembling our new students data in a temporary table and users table - " . time() . " \n";
  }
  
  watchdog("banner_integration", "routine ...import_students: Assembling our new students
                                  data in a temporary table, and also writing to the users table.");  
  
  
  
  // Now that we have our scratch table fully assembled, we will read from it, a line at a time, and
  // add what we need to add to a temporary version of our students table.  This will let us
  // quickly copy from the temporary table into our real table in the final steps.
  
  // So, let's begin by creating a temporary version of the students table.  Its declaration
  // needs to be identical to the real students table:

  db_query("CREATE TEMPORARY TABLE `TEMP_students` LIKE `students` ");
  
  // Now, we will loop through all of the CWIDs in our scratch table, figuring out whichever one
  // the student is active for (enrolled or registered = Y), and add THAT row to our TEMP_students
  // table.
  
  $use_term = reset($terms); // use the future-most term.
  // ALTERNATE:  use $cur_term for current term!
  
  
  // Begin by searching through every unique CWID
  $dres = db_query("SELECT distinct(`id`)
                    FROM banner_integration_scratch_sgvstd1
                       ");
  while ($dcur = db_fetch_array($dres)) {
    
    $use_cwid = $dcur["id"];  
  
    // Okay, find that one student's record from the scratch table, in the most-future term,
    // that is either registered or enrolled.
    $res = db_query("SELECT * 
                        FROM banner_integration_scratch_sgvstd1
                        WHERE `id` = '$use_cwid' 
                        AND term_code_key = '$use_term' 
                        AND (registered_ind = 'Y' OR enrolled_ind = 'Y')
                        ORDER BY term_code_key DESC
                        LIMIT 1");
    $cur = db_fetch_array($res);
    // Did we get anything?
    if (db_num_rows($res) == 0) {
      
      // No, so try again.  This time, just grab anything for that CWID where they're enrolled or registered,
      // from any term.
      $res = db_query("SELECT * FROM banner_integration_scratch_sgvstd1
                          WHERE `id` = '$use_cwid'
                          AND (registered_ind = 'Y' OR enrolled_ind = 'Y' OR stst_code = 'AS') 
                          ORDER BY term_code_key DESC
                          LIMIT 1");
      $cur = db_fetch_array($res);
      // Okay, do we STILL not have any records?  If not, just use ANY record this person has,
      // any term, and it doesn't matter if they are registered or enrolled
      if (db_num_rows($res) == 0) {

        $res = db_query("SELECT * FROM banner_integration_scratch_sgvstd1
                            WHERE `id` = '$use_cwid'
                            ORDER BY term_code_key DESC
                            LIMIT 1");
        $cur = db_fetch_array($res);      
      }    
    }  
    
    // Okay, if we have made it here, then $cur contains the correct row of data
    // from the scratch table, which we want to use to build our student

    // We need to fetch a few things which were not in our view.  Email and catalog year.
    
    $pidm_key = $cur["pidm_key"];
    $term_code_key = $cur["term_code_key"];
    
    if (trim($pidm_key) == "") {
      // Some kind of problem happened.  Just skip.
      continue;
    }
    
    
    // Get email and username:
    $res2 = db_query("SELECT goremal_email_address 
                             FROM banner_integration_scratch_goremal 
                             WHERE goremal_pidm = '?'
                             AND goremal_emal_code = 'ST'
                             ", $pidm_key);
    $cur2 = db_fetch_array($res2);
    $email = $cur2["goremal_email_address"];
    // TODO:  Have this username discovery configurable.
    $temp = explode("@", $email);
    
    $username = trim($temp[0]);    
    

    // Okay, now get the student's catalog year term, and discover catalog year
    $res2 = oracleapi_query("SELECT sgbstdn_term_code_ctlg_1 FROM saturn.sgbstdn
                                  WHERE sgbstdn_pidm = '$pidm_key' 
                                  AND sgbstdn_term_code_eff = sb_learner.f_query_current($pidm_key, $term_code_key)");
    $cur2 = oracleapi_fetch_array($res2); 
    $catalog_year_term = $cur2["SGBSTDN_TERM_CODE_CTLG_1"]; 
	
    $catalog_year = banner_integration_get_catalog_year_for_catalog_term($catalog_year_term);
       

    // Is this an active student?  They would be considered active if at least
    // registered_ind == Y or enrolled_ind == Y or if they were admitted in the current or future term
    $is_active = "0";
    if ($cur["registered_ind"] == "Y" || $cur["enrolled_ind"] == "Y" 
        || $cur["stst_code"] == "AS"
        || in_array($cur["term_code_admit"], $terms)) {
      $is_active = "1";
    } 

    // If the user has a concentration, we tack that on to the end of the major, as if its
    // a whole new major, with a pipe.  Ex:  ART|HIST  would be Art with a concentraion in History
    $major = trim($cur["majr_code"]);
    if (trim($cur["majr_code_concentration_1"]) != "") {
      $major .= "|" . trim($cur["majr_code_concentration_1"]);
    }
    
    // Okay, time to write to our temp table.
    db_query("INSERT INTO TEMP_students
              (cwid, cumulative_hours, gpa, rank_code, major_code, catalog_year, is_active)
              VALUES ('?', '?', '?', '?', '?', '?', '?')
              ", $cur["id"], $cur["levl_gpa_hours_earned1"], $cur["levl_gpa1"], 
                 $cur["clas_code"], $major, $catalog_year, $is_active);
    
    
    // Okay, we also want to see if this user is already in our users table.  If they are,
    // we will UPDATE the table.  If they are not, we will INSERT.
    $test_user_id = db_get_user_id_from_cwid($cur["id"], "student");
    if ($test_user_id) {
      // Yes, it's there, so let's update.
      db_query("UPDATE users
                SET user_name = '?',
                    email = '?',
                    cwid = '?',
                    f_name = '?',
                    l_name = '?'
                WHERE user_id = '?'
                ", $username, $email, $cur["id"], $cur["first_name"], $cur["last_name"], $test_user_id);
                    
    }
    else {
      // The user was NOT already there, so let's add them in.
      db_query("INSERT INTO users (user_name, email, cwid, f_name, l_name, is_student)
                VALUES ('?', '?', '?', '?', '?', '1')
                ", $username, $email, $cur["id"], $cur["first_name"], $cur["last_name"]);
    }
        
    
  }
  
  if ($bool_print_debug_messages) {
    print "\n -- Finished writing to temp table and users - " . time() . " \n";
  }
  
  watchdog("banner_integration", "routine ...import_students: Finished writing to
                                  temp table and users.");  
    
  

  ///////////////////////////////////////
  //////   Part Three: Copy temp table to real table, and truncate scratch table
  ///////////////////////////////////////

  if ($bool_print_debug_messages) {
    print "\n -- Copying temp table to real table. \n";
  }

  watchdog("banner_integration", "routine ...import_students: Copying from temp table to real table.");  
    
  
  // Okay, this should be the easy part.  We are going to truncate students, copy TEMP_students
  // to students, drop the temp table (just to be tidy), then truncate the scratch table.
  db_query("TRUNCATE students");
  db_query("INSERT students SELECT * FROM `TEMP_students` ");
  db_query("DROP TABLE `TEMP_students` ");  
  
  // Clean our scratch tables
  db_query("TRUNCATE banner_integration_scratch_sgvstd1");
  if ($bool_truncate_scratch_goremal) {
    db_query("TRUNCATE banner_integration_scratch_goremal");
  }

  
  $end_time = time();
   
  // And we're done!
  if ($bool_print_debug_messages) {
    print "\n -- Finished with import_student_users! Done in ". ($end_time - $start_time) . " seconds total.\n";
  }

  
  watchdog("banner_integration", "Finished routine ...import_students");  
    
  
  
  
} // function



/**
 * Import all student transfer courses and transfer eqv's from Banner
 *
 */
function banner_integration_routines_import_student_transfer_courses($bool_print_debug_messages = TRUE) {
  
  watchdog("banner_integration", "Starting routine ...import_student_transfer_courses");  

  // Make sure Banner is still available by grabbing the current term.  This
  // act will kill the script if it is not available.
  $cur_term_details = banner_integration_get_current_term_details();    
  
  
  $db = get_global_database_handler();
  
	$start_time = time();
	
  if ($bool_print_debug_messages) {
    print "\n -- Beginning import_student_transfer_courses routine - $start_time \n";
  }
  
  
  // Similar to the ...import_students routine, I want to make use of temporary tables,
  // since this takes so long to run, I don't want things half-broken while it runs.
  // We will write to the temp tables, then at the end, copy from temp tables to real tables.
  
  db_query("CREATE TEMPORARY TABLE `TEMP_student_transfer_courses` LIKE `student_transfer_courses` ");
  db_query("CREATE TEMPORARY TABLE `TEMP_transfer_eqv_per_student` LIKE `transfer_eqv_per_student` ");

 
  
  // To minimize calls to the database, we will keep a cache array of course ids
  $transfer_course_array = array();
  $local_course_array = array();
  $local_courses_not_found = 0;
  $skipped_rows = 0;
  
  
  // What code should we look for which means "exclude this course"?  From our settings page.
  $exclude_repeat_course_ind = variable_get("banner_integration_exclude_transfer_courses_code", "E");
  
  
  // Okay, start our (big) oracle query     
  $res = oracleapi_query("
        select spriden_id as cwid, 
               spriden_pidm as pidm,
               shrtrit_sbgi_code sbgi_institution_code, 
               shrtram_attn_period as attn_period, 
               shrtrce_term_code_eff, 
               shrtrcr_trans_course_name as transfer_subject_id,
               shrtrcr_trans_course_numbers as transfer_course_num, 
               shrtrcr_tcrse_title as transfer_course_title, 
               shrtrce_subj_code as local_subject_id, 
               shrtrce_crse_numb as local_course_num,
               shrtrcr_levl_code as level_code,
               shrtrce_credit_hours as local_hours,
               shrtrcr_trans_credit_hours as transfer_hours,
               shrtrce_grde_code as grade, 
               shrtrce_repeat_course as repeat_course_ind,
               shrtrce_term_code_eff as effective_term
        from spriden, shrtrit b, shrtram c, shrtrcr d, shrtrce e,
        scbcrse g
        where shrtrit_pidm = spriden_pidm
               
        and spriden_change_ind is null
        and shrtrit_pidm = shrtram_pidm 
        and shrtrit_pidm = shrtrcr_pidm 
        and shrtrit_pidm = shrtrce_pidm 
        and shrtrit_seq_no = shrtram_trit_seq_no
        and shrtrit_seq_no = shrtrcr_trit_seq_no
        and shrtrit_seq_no = shrtrce_trit_seq_no
        and shrtram_seq_no = shrtrcr_tram_seq_no
        and shrtram_seq_no = shrtrce_tram_seq_no
        and shrtrcr_seq_no = shrtrce_trcr_seq_no
        and shrtrce_subj_code = scbcrse_subj_code(+)
        and shrtrce_crse_numb = scbcrse_crse_numb(+)
        and shrtrce_term_code_eff = scbcrse_eff_term(+)        
                
  ");  
  

  while ($cur = oracleapi_fetch_array($res)) {
    $cur = array_change_key_case($cur);    

    // If we should skip this course, because of academic renewal (or other reasons), then we will do
    // that right now.
    if ($cur["repeat_course_ind"] == $exclude_repeat_course_ind) {
      continue;
    }
    
    
    // Let's give other contrib modules a chance to alter this row
    // For example, modules may chose to make changes to the attn_period,
    // or specify that this row should be skipped.
    // See hook_banner_integration_routines_transfer_credit_record_alter(&$db_row)
    // or the README.txt file for more information
    invoke_hook("banner_integration_routines_transfer_credit_record_alter", array(&$cur));

    // That hook allows users to make modifications, as well as other directives.
    if ($cur["SHOULD_SKIP_ROW"] == TRUE) {
      $skipped_rows++;
      continue;
    }

    extract($cur, 3, "db");
        
       
    // Is this course already in our cache array?
    $transfer_course_id = $transfer_course_array["$db_sbgi_institution_code~$db_transfer_subject_id~$db_transfer_course_num"];

    if (!$transfer_course_id || $transfer_course_id == "") {   
      // It wasn't in our cache array yet.
      
      // Try to figure out if this course is already in
      // the transfer_courses table.  If it is not, then add it.
      $transfer_course_id = db_result(
                                db_query("SELECT transfer_course_id 
                                          FROM transfer_courses
                                          WHERE institution_id = '?'
                                            AND subject_id = '?'
                                            AND course_num = '?'
                                          LIMIT 1", $db_sbgi_institution_code, $db_transfer_subject_id,
                                                    $db_transfer_course_num));
      if (!$transfer_course_id || $transfer_course_id < 1) {
        // It was NOT found.  So, let's add it to the table.
        db_query("INSERT INTO transfer_courses (institution_id, subject_id, course_num, title)
                  VALUES ('?', '?', '?', '?')
                  ", $db_sbgi_institution_code, $db_transfer_subject_id, $db_transfer_course_num,
                     $db_transfer_course_title);
                     
        $transfer_course_id = db_insert_id();
                  
       
      }
      // Add to our cache array
      $transfer_course_array["$db_sbgi_institution_code~$db_transfer_subject_id~$db_transfer_course_num"] = $transfer_course_id;
    }
    
    if ($cur["NO_LOCAL_EQV"] != TRUE) {
      // Okay, now get the local course's ID...
      $local_course_id = $local_course_array["$db_local_subject_id~$db_local_course_num"];
      if (!$local_course_id || $local_course_id == "") {
        // Couldn't find it, try to locate from the database.
        $local_course_id = $db->get_course_id($db_local_subject_id, $db_local_course_num);
        if (!$local_course_id) {
          // Still couldn't find it!  
          $local_courses_not_found++;
          //continue;  // we don't want to skip it; just keep going and don't add it to our
                       // array
        }
        else {
          $local_course_array["$db_local_subject_id~$db_local_course_num"] = $local_course_id;
        }
      }
    }
    
    // Now that I have the transfer_course_id and local_course_id, 
    // I can proceed with adding in the EQV's and
    // giving the student credit for the course.

    
    // Figure out, what is the "broken_id", which is terribly named!  It should be 0
    // by default, and non-zero if this can't be a straight 1:1 eqv.  In other words,
    // if the course is eqv to several local courses.
    
    // To test this, we will use the banner_integration_get_sbgi_institution_multiplier() function.
    $broken_id = 0;
    $multiplier = banner_integration_get_sbgi_institution_multiplier($db_sbgi_institution_code, $db_effective_term);
    if (($db_transfer_hours * $multiplier) > $db_local_hours) {
      $broken_id = 1;
    } else if (($db_transfer_hours * $multiplier) < $db_local_hours) {
      $broken_id = -1;
    }    

    
    // If this course has a non-zero "broken_id", that means that it has been split up in Banner.
    // But in FP, we want to re-combine the course into itself.
    // So, we will query for if the course is ALREADY in our table.  If so, we want it's hours, so we
    // can add them to this records.  We will then delete the existing record, and re-add below with
    // the updated hours, so that we don't have more than one row in our student_transfer_courses
    // table.
    if ($broken_id != 0) {
      $res2 = db_query(" SELECT `id`, hours_awarded 
                         FROM `TEMP_student_transfer_courses`
                         WHERE student_id = '?'
                         AND transfer_course_id = '?'
                         AND term_id = '?'
                              ", $db_cwid, $transfer_course_id, $db_attn_period);
      $cur2 = db_fetch_array($res2);
      $existing_id = $cur2["id"];
      $existing_hours = $cur2["hours_awarded"];
      
      if (intval($existing_hours) > 0) {
        // Add to the next row's hour count...
        $db_local_hours += intval($existing_hours);
        // Delete the row that's there...
          
        db_query("DELETE FROM `TEMP_student_transfer_courses`
                  WHERE `id` = '?' ", $existing_id);
      }
      
    }
    

    // Let's simply giving the student credit for this course, by adding it to the
    // student_transfer_courses table.
    db_query("INSERT INTO `TEMP_student_transfer_courses`
                (student_id, transfer_course_id, student_specific_course_title, term_id,
                 grade, hours_awarded, level_code)
              VALUES ('?', '?', '?', '?', '?', '?', '?')
              ", $db_cwid, $transfer_course_id, $db_transfer_course_title, $db_attn_period,
                  $db_grade, $db_local_hours, $db_level_code);
    
    // Okay, now that that's inserted, we can add in the eqv (if there is one)
    if ($cur["NO_LOCAL_EQV"] != TRUE) {
      // There IS a local eqv.  Add it to the table!
      
      // Okay, ready to insert
      db_query("INSERT INTO `TEMP_transfer_eqv_per_student`
                  (student_id, transfer_course_id, local_course_id, valid_term_id, broken_id)
                VALUES ('?', '?', '?', '?', '?')
                ", $db_cwid, $transfer_course_id, $local_course_id, $db_effective_term, $broken_id);
      
      
    }                  
       
    
  }
  
  /////////////////
  // Okay, we can now write to our real tables.
  watchdog("banner_integration", "routine ...import_student_transfer_courses: Copying from temp
                                  tables to real tables.");  
  
  db_query("TRUNCATE TABLE student_transfer_courses");
  db_query("INSERT student_transfer_courses SELECT * FROM `TEMP_student_transfer_courses` ");
  db_query("DROP TABLE `TEMP_student_transfer_courses` ");    
  
  
  db_query("TRUNCATE TABLE transfer_eqv_per_student");   
  db_query("INSERT transfer_eqv_per_student SELECT * FROM `TEMP_transfer_eqv_per_student` ");
  db_query("DROP TABLE `TEMP_transfer_eqv_per_student` ");    
   
  
  /////////////////////
  
  $end_time = time();
   
  // And we're done!
  if ($bool_print_debug_messages) {
    print "\n $local_courses_not_found local courses weren't found. $c \n";
    print "\n $skipped_rows rows were skipped. \n";
    print "\n -- Finished with import_student_transfer_courses! Done in ". ($end_time - $start_time) . " seconds total.\n";
  }
   
  watchdog("banner_integration", "Finished routine ...import_student_transfer_courses.
             @local local courses (eqvs) weren't found.
             @skipped rows were skipped.",
              array("@local" => $local_courses_not_found, "@skipped" => $skipped_rows));
  
  
} // function


//CODE-ADD-START - STUDENT COURSES (student_courses)
/**
 * This routine populates FP's student_courses table with student course data from Banner.
 * 
 * We will look at Banner's SHRTCKN, SHRTCKG, and SGBSTDN tables to populate the student_courses (meaning,
 * non-transfer) courses in FlightPath.  
 *
 */
 function banner_integration_routines_import_student_courses() 
 {
 watchdog("banner_integration", "Starting routine ...import_student_courses");
 
 db_query("TRUNCATE student_courses");
 $db = get_global_database_handler();
 $res = oracleapi_query("	SELECT	UOS_PIDM_ID(KN.SHRTCKN_PIDM)	STUDENT_ID,
									KN.SHRTCKN_SUBJ_CODE			SUBJECT_ID,
									KN.SHRTCKN_CRSE_NUMB			COURSE_NUM,
									OUKG.SHRTCKG_CREDIT_HOURS		HOURS_AWARDED,
									OUKG.SHRTCKG_GRDE_CODE_FINAL	GRADE,
									KN.SHRTCKN_TERM_CODE			TERM_ID,
									A.SGBSTDN_LEVL_CODE				LEVEL_CODE
							FROM	SHRTCKN KN, SHRTCKG OUKG, SGBSTDN A
							WHERE	KN.SHRTCKN_PIDM							=	OUKG.SHRTCKG_PIDM
							AND		KN.SHRTCKN_TERM_CODE||KN.SHRTCKN_SEQ_NO	=	OUKG.SHRTCKG_TERM_CODE||OUKG.SHRTCKG_TCKN_SEQ_NO
							AND 	OUKG.SHRTCKG_SEQ_NO 					= 	(	SELECT	MAX (INNKG.SHRTCKG_SEQ_NO)
																					FROM	SHRTCKG INNKG
																					WHERE 	INNKG.SHRTCKG_PIDM = OUKG.SHRTCKG_PIDM
																					AND 	INNKG.SHRTCKG_TERM_CODE||INNKG.SHRTCKG_TCKN_SEQ_NO = OUKG.SHRTCKG_TERM_CODE||OUKG.SHRTCKG_TCKN_SEQ_NO
																				)
							AND		KN.SHRTCKN_PIDM							=	A.SGBSTDN_PIDM
							AND		A.SGBSTDN_STST_CODE       				IN 	('AS','AW','CW','DA','DW','OW','PA','RA','SP','WN','XP','XT')
							AND		A.SGBSTDN_TERM_CODE_EFF					= 	(	SELECT   MAX(B.SGBSTDN_TERM_CODE_EFF)
																					FROM     SGBSTDN B
																					WHERE    B.SGBSTDN_PIDM 			=	A.SGBSTDN_PIDM
																					AND      B.SGBSTDN_TERM_CODE_EFF	<= 	to_char(sysdate, 'YYYY') || '30'  
																				)
						");
			
	 while ($cur = oracleapi_fetch_array($res)) 
	 {			
		$student_id = $cur["STUDENT_ID"];
		$subject_id = $cur["SUBJECT_ID"];
		$course_num = $cur["COURSE_NUM"];
		$hrs_award  = $cur["HOURS_AWARDED"];
		$grade     = $cur["GRADE"];
		$term_id    = $cur["TERM_ID"];
		$level_code = $cur["LEVEL_CODE"];
		
		db_query("INSERT INTO student_courses
			(student_id, subject_id, course_num, hours_awarded, grade, term_id, level_code) 
			VALUES 
			('?', '?', '?', '?', '?', '?', '?')", $student_id, $subject_id, $course_num, $hrs_award, $grade, $term_id, $level_code);
	 }
  // All done!
  watchdog("banner_integration", "Finished routine ...import_student_courses");  
 }
//CODE-ADD-END
 
//CODE-ADD-START- IMPORT SUBJECTS
 /**
 * This routine populates FP's subjects table
 * 
 * Source tables are SCBCRSE and STVSUBJ
 *
 */
 function banner_integration_routines_import_subjects() 
 {
 watchdog("banner_integration", "Starting routine ...import_subjects");
 
 $db = get_global_database_handler();
 $res = oracleapi_query("SELECT	DISTINCT 
								A.SCBCRSE_SUBJ_CODE 																		SUBJECT_ID,
								A.SCBCRSE_COLL_CODE																			COLLEGE,
								TRIM(SUBSTR((SELECT STVSUBJ_DESC FROM STVSUBJ WHERE STVSUBJ_CODE = SCBCRSE_SUBJ_CODE), 5))	TITLE
						FROM	SCBCRSE A 
						WHERE	A.SCBCRSE_CSTA_CODE	=	'A'
						AND		A.SCBCRSE_EFF_TERM  =	(	SELECT  MAX(B.SCBCRSE_EFF_TERM)
															FROM    SCBCRSE B 
															WHERE   B.SCBCRSE_SUBJ_CODE = A.SCBCRSE_SUBJ_CODE
															AND     B.SCBCRSE_CRSE_NUMB = A.SCBCRSE_CRSE_NUMB
														)
						");
 
	 while ($cur = oracleapi_fetch_array($res)) 
	 {			
		
		$subject_id = $cur["SUBJECT_ID"];
		$college = $cur["COLLEGE"];
		$title  = $cur["TITLE"];
		
		db_query("INSERT INTO subjects
			(subject_id, college, title) 
			VALUES 
			('?', '?', '?')", $subject_id, $college, $title);
		
	 }
	 
  // All done!
  watchdog("banner_integration", "Finished routine ...import_subjects");  
 }

//CODE-ADD-END

//CODE-ADD-START- IMPORT COLLEGES
 /**
 * This routine populates FP's colleges table
 * 
 * Source table is STVCOLL
 *
 */
 function banner_integration_routines_import_colleges() 
 {
 watchdog("banner_integration", "Starting routine ...import_colleges");
 
 $db = get_global_database_handler();
 $res = oracleapi_query("SELECT	DISTINCT 
								STVCOLL_CODE	COLLEGE_CODE,
								STVCOLL_DESC	TITLE
						FROM	STVCOLL
						");
						
	 while ($cur = oracleapi_fetch_array($res)) 
	 {			
		
		$college_code = $cur["COLLEGE_CODE"];
		$title  = $cur["TITLE"];
		
		db_query("INSERT INTO colleges
			(college_code, title) 
			VALUES 
			('?', '?')", $college_code, $title);
		
	 }
	 
  // All done!
  watchdog("banner_integration", "Finished routine ...import_colleges");  
 }

//-------CODE-ADD-END

//CODE-ADD-START -2017XXXX New Routine To fetch the student test scores and update the student_tests table
function banner_integration_routines_import_test_courses() 
 {
 watchdog("banner_integration", "Starting routine ...import_student_tests");
 
 $db = get_global_database_handler();
 $res = oracleapi_query("SELECT		UOS_PIDM_ID(A.SORTEST_PIDM)	STUDENT_ID,
                                  SORTEST_TESC_CODE			      TEST_ID,
                                  SORTEST_ADMR_CODE			      CATEGORY_ID,
                                  SORTEST_TEST_SCORE			    SCORE,
                                  TO_CHAR(SORTEST_TEST_DATE, 'DD-MM-YYYY') DATE_TAKEN
                          FROM		SORTEST A, SGBSTDN B 
                          WHERE		A.SORTEST_PIDM			=	B.SGBSTDN_PIDM
                          AND			B.SGBSTDN_STST_CODE		IN 	('AS','AW','CW','DA','DW','OW','PA','RA','SP','WN','XP','XT' )
                          AND			B.SGBSTDN_TERM_CODE_EFF	= 	(	SELECT   MAX(B1.SGBSTDN_TERM_CODE_EFF)
                                                                FROM     SGBSTDN B1
                                                                WHERE    B1.SGBSTDN_PIDM	= B.SGBSTDN_PIDM
                                          										)
                      ");
						
	 while ($cur = oracleapi_fetch_array($res)) 
	 {			
		
      $student_id = $cur["STUDENT_ID"];
      $test_id    = $cur["TEST_ID"];
      $category_id= $cur["CATEGORY_ID"];
      $score      = $cur["SCORE"];
      $date_taken   = $cur["DATE_TAKEN"];
   
      
		db_query("INSERT INTO student_tests
			(student_id, test_id, category_id, score, date_taken) 
			VALUES 
			('?', '?', '?', '?', STR_TO_DATE('?','%d-%m-%Y'))", $student_id, $test_id, $category_id, $score, $date_taken);
		
	 }
	 
  // All done!
  watchdog("banner_integration", "Finished routine ...import_student_tests");  
 }
//CODE-ADD-END



