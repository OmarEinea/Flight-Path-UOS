<?php

/**
 * @file
 * Keep track of functions dealing specifically with student user management
*/


/**
 * This is the form which we will use to manage a student's courses they've taken.
 */
function user_student_edit_student_courses_form() {
    
  fp_add_js(fp_get_module_path("user") . "/js/user.js");

  $form = array();
  $student_cwid = $_REQUEST["student_cwid"];  // now getting it from argument
  $user_id = db_get_user_id_from_cwid($student_cwid, "student");



  // Figure out what the page's sub-tabs should be, and set them.
  $tab_array = array();
  $tab_array[0]["title"] = t("trarabic_10216");
  $tab_array[0]["active"] = FALSE;
  $tab_array[0]["on_click"] = "window.location=\"" . fp_url("admin/users/edit-student-user", "student_cwid=$student_cwid&de_catalog_year=$de_catalog_year") . "\"";
      
  $tab_array[1]["title"] = t("trarabic_10217");
  $tab_array[1]["active"] = TRUE;
  $tab_array[1]["on_click"] = "window.location=\"" . fp_url("admin/users/edit-student-user/courses", "student_cwid=$student_cwid&de_catalog_year=$de_catalog_year") . "\"";
      
  fp_set_page_sub_tabs($tab_array);




  
  if ($student_cwid != "new") {  
    $name = fp_get_student_name($student_cwid);  
    fp_set_title(t("trarabic_10218", array("@name" => $name, "@id" => $student_cwid)));
  }
  else {
    // A new student!  We can't create a new student until a CWID is assigned.
    fp_set_title(t("trarabic_10219"));
    $form["mark_sorry"] = array(
      "type" => "markup",
      "value" => "<p>" . t("trarabic_20043") . "</p>",
    );
    return $form;
  }

  $form["user_id"] = array(
    "type" => "hidden",
    "value" => $user_id,
  );

  $form["perform_action2"] = array(
    "type" => "hidden",
    "value" => "",
  );

  $form["student_cwid"] = array(
    "type" => "hidden",
    "value" => $student_cwid,
  );
  
  
  
  // Okay, let's create a table in which we will place the student's courses.
  $form["mark" . $m++] = array(
    "value" => "<p>
                  " . ("Enter the student's local (non-transfer) courses they have enrolled in at your institution.") . "
                  <ul>
                    <li>" . t("trarabic_10220") . "</li>
                    <li>" . t("trarabic_10221") . "</li>
                    <li>" . t("trarabic_10222") . "</li>
                    <li>" . t("trarabic_10223") . "</li>
                    <li>" . t("trarabic_10224") . "</li>
                  </ul>
                  " . t("trarabic_20044") . "
                </p>",
  );
  
  $form["warn_me"] = array(
    "type" => "checkbox",
    "label" => t("trarabic_10225"),    
    "value" => "yes",
  );
  
  
  $form["mark" . $m++] = array(
    "value" => "
                <table border='0' width='100%' cellpadding='3' cellspacing='0'>
                  <tr>
                    <th>" . t("trarabic_10226") . "</th>
                    <th>" . t("trarabic_10227") . "</th>
                    <th>" . t("trarabic_10228") . "</th>
                    <th>" . t("trarabic_10229") . "</th>
                    <th>" . t("trarabic_10230") . "</th>
                  </tr>",
  );
  
  
  // Let's get an array of what courses the student has already taken (if any)
  $courses = array();
  $res = db_query("SELECT * FROM student_courses 
                   WHERE student_id = '?'
                   ORDER BY subject_id, course_num", $student_cwid);
  while ($cur = db_fetch_array($res)) {
    $courses[] = array(
      "course" => $cur["subject_id"] . " " . $cur["course_num"],
      "term" => $cur["term_id"],
      "grade" => $cur["grade"],
      "hrs" => $cur["hours_awarded"] * 1, // will trim excess zeroes if there
      "lvl" => $cur["level_code"],
    );
  }                   
              
  // Let's add 10 additional (blank) lines to the courses array.
  for ($t = 0; $t < 10; $t++) {
    $courses[] = array(
      "course" => "",
      "term" => "",
      "grade" => "",
      "hrs" => "",
      "lvl" => "",
    );    
  }
  
  
  // Okay, now let's go through the courses array and display in the table...
  
  $stripe = "";  
  foreach ($courses as $t => $val) {
        
    if ($stripe == "") {
      $stripe = "style='background-color: beige;'";
    }
    else {
      $stripe = "";
    }  
        
    $form["course_$t"] = array(
      "prefix" => "<tr $stripe >
                      <td valign='top'>",
      "type" => "textfield",
      "size" => 12,      
      "suffix" => "</td>",
      "value" => $val["course"],
    );    

    $form["term_$t"] = array(
      "prefix" => "<td valign='top'>",
      "type" => "textfield",
      "size" => 8,      
      "suffix" => "</td>",
      "value" => $val["term"],
    );    

    $form["grade_$t"] = array(
      "prefix" => "<td valign='top'>",
      "type" => "textfield",
      "size" => 4,      
      "suffix" => "</td>",
      "value" => $val["grade"],
    );    
    
    $form["hrs_$t"] = array(
      "prefix" => "<td valign='top'>",
      "type" => "textfield",
      "size" => 4,      
      "suffix" => "</td>",
      "value" => $val["hrs"],
    );    
                
    $form["lvl_$t"] = array(
      "prefix" => "<td valign='top'>",
      "type" => "textfield",
      "size" => 4,      
      "suffix" => "</td>
                   </tr>",
      "value" => $val["lvl"],                   
    );    
                    
    
  } // for loop
   
  
  $form["number_of_courses"] = array(
    "type" => "hidden",
    "value" => count($courses),
  );
  
  
  
  // close table.
  $form["mark" . $m++] = array(
    "value" => "</table>
                <p>" . t("trarabic_20045") . "</p>",
  );
  

  $form["submit_btn"] = array(
    "type" => "submit",
    "value" => "Submit",
  );  
  
  
  return $form;
} // user_student_edit_student_courses_form


function user_student_edit_student_courses_form_validate($form, $form_state) {
  // If the user requested it, check to see if the course exists (in the draft_courses table)
   
  if ($form_state["values"]["warn_me"] === TRUE) {
    // Yes, the user wants to be warned if they entered a course which doesn't exist.
    $number_of_courses = intval($form_state["values"]["number_of_courses"]);
    for ($t = 0; $t < $number_of_courses; $t++) {
      $course = trim($form_state["values"]["course_$t"]);
      if ($course == "") continue;
      
      $temp = explode(" ", $course);
      $subject_id = trim($temp[0]);
      $course_num = trim($temp[1]);
      
      // Check to see that this course exists.
      $res = db_query("SELECT subject_id FROM draft_courses
                       WHERE subject_id = '?'
                       AND course_num = '?' ", $subject_id, $course_num);
      $cur = db_fetch_array($res);
      if ($cur["subject_id"] != $subject_id) {
        form_error("course_$t", t("trarabic_10231", 
                                                  array("%subject_id" => $subject_id, "%course_num" => $course_num)));
      }                       
                
    }
  }
  
  
  
}





function user_student_edit_student_courses_form_submit($form, $form_state) {
    
  $student_id = $form_state["values"]["student_cwid"];
  
  // Erase what's there already:
  db_query("DELETE FROM student_courses WHERE student_id = '?' ", $student_id);  
  
  $number_of_courses = intval($form_state["values"]["number_of_courses"]);
  for ($t = 0; $t < $number_of_courses; $t++) {
    $course = trim($form_state["values"]["course_$t"]);
    if ($course == "") continue;
    
    $temp = explode(" ", $course);
    $subject_id = trim($temp[0]);
    $course_num = trim($temp[1]);      
    
    $term_id = trim($form_state["values"]["term_$t"]);
    $grade = trim($form_state["values"]["grade_$t"]);
    $hours_awarded = trim($form_state["values"]["hrs_$t"]);
    $level_code = trim($form_state["values"]["lvl_$t"]);

    // Add to table
    db_query("INSERT INTO student_courses (student_id, subject_id, course_num, hours_awarded, grade, term_id, level_code)
              VALUES ('?', '?', '?', '?', '?', '?', '?')", $student_id, $subject_id, $course_num, $hours_awarded, $grade, $term_id, $level_code);
   
  }  

  fp_add_message(t("trarabic_10232"));
  
    
} // student_courses_form_submit






/**
 * Let the user edit a studentuser's information.
 */
function user_edit_student_user_form() {

  fp_add_js(fp_get_module_path("user") . "/js/user.js");

  $form = array();
  $student_cwid = $_REQUEST["student_cwid"];  // now getting it from argument
  $user_id = db_get_user_id_from_cwid($student_cwid, "student");



  // Figure out what the page's sub-tabs should be, and set them.
  $tab_array = array();
  $tab_array[0]["title"] = t("trarabic_10233");
  $tab_array[0]["active"] = TRUE;
  $tab_array[0]["on_click"] = "window.location=\"" . fp_url("admin/users/edit-student-user", "student_cwid=$student_cwid&de_catalog_year=$de_catalog_year") . "\"";
      
  $tab_array[1]["title"] = t("trarabic_10234");
  $tab_array[1]["active"] = FALSE;
  $tab_array[1]["on_click"] = "window.location=\"" . fp_url("admin/users/edit-student-user/courses", "student_cwid=$student_cwid&de_catalog_year=$de_catalog_year") . "\"";
      
  fp_set_page_sub_tabs($tab_array);





  
  if ($student_cwid != "new") {  
    $name = fp_get_student_name($student_cwid);  
    fp_set_title(t("trarabic_10235", array("@name" => $name, "@id" => $student_cwid)));
  }
  else {
    // A new student!
    fp_set_title(t("trarabic_10236"));
  }

  $form["user_id"] = array(
    "type" => "hidden",
    "value" => $user_id,
  );

  $form["perform_action2"] = array(
    "type" => "hidden",
    "value" => "",
  );

  $form["student_cwid"] = array(
    "type" => "hidden",
    "value" => $student_cwid,
  );
    
  
  // TODO: At the moment, only faculty can be assigned roles in FP.  However, this could change
  // one day, so I am going to leave this code in place for students, but commented out.
  /*
  $user_roles = system_get_roles_for_user($user_id);
  //fpm($user_roles);
  $default_values = array();
  foreach ($user_roles as $rid => $val) {
    $default_values[$rid] = $rid;
  }

  // Show a list of roles in the system which we may select from, and check the ones
  // all ready assigned to this user.
  
  $options = array();
      
  $res = db_query("SELECT * FROM roles ORDER BY rid");
  while ($cur = db_fetch_array($res)) {
    
    $key = $cur["rid"];
    $value = $cur["name"];

    if ($key > 2) {
      $options[$key] = $value;
    } 
      
  }  
//fpm($default_values);
  $form["roles"] = array(
    "label" => t("trarabic_10237"),
    "type" => "checkboxes",
    "options" => $options,
    "value" => $default_values,
  );
  */  


  // Let's present the form elements to allow some basic editing of this user.
  
  // Only if we are making a new student...
  if ($student_cwid == "new") {
    $form["new_student_cwid"] = array(
      "label" => t("trarabic_10238"),
      "type" => "textfield",
      "size" => 20,
      "required" => TRUE,     
      "description" => t("trarabic_20046"),
    );
    
    $form["new_user_name"] = array(
      "label" => t("trarabic_10239"),
      "type" => "textfield",
      "size" => 20,     
      "required" => TRUE,
      "description" => t("trarabic_20047"),
    );
        
    
    $cur = array();  
  }
  else {
    // NOT a new student.  Load their information normally.
    
    $res = db_query("SELECT * FROM users u, students s
                     WHERE u.cwid = '?'
                     AND u.is_student = '1'
                     AND u.cwid = s.cwid", $student_cwid);
    $cur = db_fetch_array($res);
  }

  $user_name = $cur["user_name"];
  if ($user_name != "") {
    $form["mark" . $m++] = array(
      "value" => "<p><b>Username:</b> $user_name</p>",
    );
  }
  
  $form["new_password"] = array(
    "label" => t("trarabic_10240"),
    "type" => "textfield",
    "size" => 20,
    "required" => ($student_cwid == "new") ? TRUE : FALSE,
    "description" => t("trarabic_20048"),    
  );

  $form["email"] = array(
    "label" => t("trarabic_10241"),
    "type" => "textfield",
    "value" => $cur["email"],
  );  

  $form["f_name"] = array(
    "label" => t("trarabic_10242"),
    "type" => "textfield",
    "value" => $cur["f_name"],
  );  

  $form["l_name"] = array(
    "label" => t("trarabic_10243"),
    "type" => "textfield",
    "value" => $cur["l_name"],
  );  

  $form["is_disabled"] = array(
    "label" => t("trarabic_10244"),
    "type" => "textfield",
    "value" => $cur["is_disabled"],
    "size" => 5,
    "description" => t("trarabic_20049"),
  );  
  
  
  // Unique to students...
  $form["cumulative_hours"] = array(
    "label" => t("trarabic_10245"),
    "type" => "textfield",
    "value" => $cur["cumulative_hours"],
    "size" => 5,
    "description" => t("trarabic_20050"),
  );  
    
  $form["gpa"] = array(
    "label" => t("trarabic_10246"),
    "type" => "textfield",
    "value" => $cur["gpa"],
    "size" => 5,
    "description" => t("trarabic_20051"),    
  );  
    
  $form["rank_code"] = array(
    "label" => t("trarabic_10247"),
    "type" => "textfield",
    "value" => $cur["rank_code"],
    "size" => 5,
    "description" => t("trarabic_10248") . "
                      <br><b>" . t("trarabic_10249") . "</b> " . t("trarabic_20052") . "<br>&nbsp; &nbsp; &nbsp;" . t("trarabic_10250") . " 
                      <i>" . variable_get("allowed_student_ranks", "") . "</i>",
  );  

  $form["major_code"] = array(
    "label" => t("trarabic_10251"),
    "type" => "textfield",
    "value" => $cur["major_code"],
    "size" => 10,
  );  
    
  $form["catalog_year"] = array(
    "label" => t("trarabic_10252"),
    "type" => "textfield",
    "value" => $cur["catalog_year"],
    "size" => 10,
    "description" => t("trarabic_20053"),
  );  

  $form["is_active"] = array(
    "label" => t("trarabic_10253"),
    "type" => "textfield",
    "value" => $cur["is_active"],
    "size" => 5,
    "description" => t("trarabic_20054"),
  );  
        
      
      
  
  $form["submit"] = array(
    "type" => "submit",  
    "value" => "Submit",    
    "prefix" => "<hr>",
  );
  
  
  if ($student_cwid != "new" && user_has_permission("delete_users")) {
    
    $form["mark" . $m++] = array(
      "type" => "markup",
      "value" => "<div align='right'>
                        " . t("trarabic_10254") . " <input type='button' value='X'
                        onClick='userDeleteStudent();'>
                      </div>", 
    );  
  }
  
  
  
  
  
  return $form;    
  
}


/**
 * Validate handler for editing student users.
 */
function user_edit_student_user_form_validate($form, $form_state) {
  
  $values = $form_state["values"];
  
  // If a password was given, make sure it is appropriate.
  if (trim($values["new_password"]) != "") {
    if (strlen(trim($values["new_password"])) < 5) {
      form_error("new_password", t("trarabic_10255"));
      return;
    }
  }
  
  // If creating a new user, make sure new_student_cwid and new_user_name are not
  // already in use.
  if ($values["student_cwid"] == "new") {
    $new_cwid = trim($values["new_student_cwid"]);
    $new_user_name = trim($values["new_user_name"]);

    /*  // CWIDS are no longer required to be numeric.
    // Check that cwid is numeric.
    if (!is_numeric($new_cwid)) {
      form_error("new_student_cwid", t("trarabic_20055"));
      return;
    }
    */

    // Check that username is at least 4 characters
    if (strlen($new_user_name) < 4) {
      form_error("new_user_name", t("trarabic_20056"));
      return;
      
    }

    
    // Check cwid isn't already in use.
    $test = db_result(db_query("SELECT cwid FROM users WHERE cwid = '?' AND is_student = '1'", $new_cwid));    
    if ($test == $new_cwid) {
      form_error("new_student_cwid", t("trarabic_10256"));
      return;
    }
   
    // Check user_name isn't already in use.
    $test = db_result(db_query("SELECT user_name FROM users WHERE user_name = '?' ", $new_user_name));    
    if ($test == $new_user_name) {
      form_error("new_user_name", t("trarabic_10257"));
      return;
    }
      
   
    
  }
    
  
}


/**
 * Submit handler for editing student users.
 */
function user_edit_student_user_form_submit($form, $form_state) {
  
  $values = $form_state["values"];
  foreach ($values as $key => $val) {
    if (!is_array($val)) {
      $values[$key] = trim($val);
    }
  }
  //fpm($values);
  $user_id = $values["user_id"];
  $student_cwid = $values["student_cwid"];
  
  // Are we supposed to DELETE a student?
  if ($values["perform_action2"] == "delete_student" && user_has_permission("delete_users")) {

    
    db_query("DELETE FROM students WHERE cwid = '?' ", $student_cwid);
    db_query("DELETE FROM users WHERE cwid = '?' AND is_student = '1' ", $student_cwid);      
    
    fp_add_message(t("trarabic_10258"));
    fp_goto("admin/users/students");
    return;
  }
  
  
  if ($student_cwid != "new") {
    // NOT a new student!  Insert values normally.
    // First-- was there a password given?  If so, insert that separate.
    if (trim($values["new_password"]) != "") {
      $new_pass = md5(trim($values["new_password"]));
      db_query("UPDATE users 
                SET password = '?' 
                WHERE cwid = '?' 
                AND is_student = '1' ", $new_pass, $student_cwid);
    }  
    // Okay, now we can just update everything else.
    // Update users table first...
    db_query("UPDATE users
              SET email = '?',                  
                  f_name = '?',
                  l_name = '?',
                  is_disabled = '?'
               WHERE cwid = '?'
               AND is_student = '1' ", $values["email"], $values["f_name"], 
                                       $values["l_name"], $values["is_disabled"],
                                       $student_cwid);
   
   // Now, update the students table entry.
   db_query("UPDATE students
             SET cumulative_hours = '?',
                 gpa = '?',
                 rank_code = '?',
                 major_code = '?',
                 catalog_year = '?',
                 is_active = '?'                 
             WHERE cwid = '?'", $values["cumulative_hours"], $values["gpa"], $values["rank_code"],
                                $values["major_code"], $values["catalog_year"], 
                                $values["is_active"], $student_cwid);             

  }
  else {
    // This is a NEW user!  We need to perform inserts.  Thanks to our validate handler,
    // we know all of the values we have are valid.
    db_query("INSERT INTO users (user_name, password, is_student, email, cwid, f_name, l_name, is_disabled)
              VALUES ('?', '?', '1', '?', '?', '?', '?', '?')
              ", $values["new_user_name"], md5($values["new_password"]), $values["email"], $values["new_student_cwid"],
                 $values["f_name"], $values["l_name"], $values["is_disabled"]);
    db_query("INSERT INTO students (cwid, cumulative_hours, gpa, rank_code, major_code, catalog_year, is_active)
              VALUES ('?', '?', '?', '?', '?', '?', '?')
             ", $values["new_student_cwid"], $values["cumulative_hours"], $values["gpa"], $values["rank_code"],
                $values["major_code"], $values["catalog_year"], $values["is_active"]);
                                  
    fp_add_message(t("trarabic_10259"));
    fp_goto("admin/users/edit-student-user", "student_cwid=" . $values["new_student_cwid"]);
  }
  
  
      
  fp_add_message(t("trarabic_10260"));
  
  
}

