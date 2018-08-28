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
  $tab_array[0]["title"] = t("تعديل الطالب");
  $tab_array[0]["active"] = FALSE;
  $tab_array[0]["on_click"] = "window.location=\"" . fp_url("admin/users/edit-student-user", "student_cwid=$student_cwid&de_catalog_year=$de_catalog_year") . "\"";
      
  $tab_array[1]["title"] = t("تعديل مساقات الطالب");
  $tab_array[1]["active"] = TRUE;
  $tab_array[1]["on_click"] = "window.location=\"" . fp_url("admin/users/edit-student-user/courses", "student_cwid=$student_cwid&de_catalog_year=$de_catalog_year") . "\"";
      
  fp_set_page_sub_tabs($tab_array);




  
  if ($student_cwid != "new") {  
    $name = fp_get_student_name($student_cwid);  
    fp_set_title(t(" @name (@id) تعديل مساقات الطالب", array("@name" => $name, "@id" => $student_cwid)));
  }
  else {
    // A new student!  We can't create a new student until a CWID is assigned.
    fp_set_title(t("إضافة حساب طالب جديد"));
    $form["mark_sorry"] = array(
      "type" => "markup",
      "value" => "<p>" . t(".عذرًا، ولكن لا يمكنك إضافة مقررات الطالب حتى يتم إنشاء الطالب بشكل كامل (ويعطى رمز المستخدم). استخدم زر تعديل الطالب أعلاه للرجوع إلى تلك الشاشة") . "</p>",
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
                    <li>" . t("ACCT 101 :المساق: أدخل رمز المساق ورقمه مفصول بمسافة. مثال") . "</li>
                    <li>" . t("201540 :أدخل رمز الفصل الذي سجل فيه هذا الطالب في هذا المساق. مثال") . "</li>
                    <li>" . t("B :العلامة: أدخل العلامة التي استحقها الطالب. مثال") . "</li>
                    <li>" . t(" 3 :الساعات: أدخل العدد الساعات المستحقة. مثال") . "</li>
                    <li>" . t("UG :الدرجة -اختياري- أدخل الدرجة لهذا المساق. مثال") . "</li>
                  </ul>
                  " . t("فسيتم تجاهل هذه البيانات أو حذفها (student_courses إذا كان لديك أي إجراءات قد تستبدل هذه البيانات (في جدول<b>:ملاحظة</b>") . "
                </p>",
  );
  
  $form["warn_me"] = array(
    "type" => "checkbox",
    "label" => t("حذرني إذا قمت بإدخال دورة غير موجودة في draft_courses (جيد للقبض على الأخطاء المطبعية)"),    
    "value" => "yes",
  );
  
  
  $form["mark" . $m++] = array(
    "value" => "
                <table border='0' width='100%' cellpadding='3' cellspacing='0'>
                  <tr>
                    <th>" . t("المساق") . "</th>
                    <th>" . t("الفصل") . "</th>
                    <th>" . t("العلامة") . "</th>
                    <th>" . t("الساعات") . "</th>
                    <th>" . t("المرحلة") . "</th>
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
                <p>" . t(".سيتم خطي الصفوف التي لا تحتوي على مساقات. لإضافة المزيد من الصفوف الفارغة, احفظ عملك. عند إعادة تحميل الصفحة, يكون هناك صفوف إضافية فارغة لإضافة مساقات جديدة") . "</p>",
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
        form_error("course_$t", t("لم يتم حفظ بياناتك .draft_courses في جدول ال %subject_id %course_num لم بتم العثور على المساق", 
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

  fp_add_message(t("تم تحديث مساقات الطالب"));
  
    
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
  $tab_array[0]["title"] = t("عدل الطالب");
  $tab_array[0]["active"] = TRUE;
  $tab_array[0]["on_click"] = "window.location=\"" . fp_url("admin/users/edit-student-user", "student_cwid=$student_cwid&de_catalog_year=$de_catalog_year") . "\"";
      
  $tab_array[1]["title"] = t("عدل مساقات الطالب");
  $tab_array[1]["active"] = FALSE;
  $tab_array[1]["on_click"] = "window.location=\"" . fp_url("admin/users/edit-student-user/courses", "student_cwid=$student_cwid&de_catalog_year=$de_catalog_year") . "\"";
      
  fp_set_page_sub_tabs($tab_array);





  
  if ($student_cwid != "new") {  
    $name = fp_get_student_name($student_cwid);  
    fp_set_title(t("@name (@id) عدل المستخدم الطالب", array("@name" => $name, "@id" => $student_cwid)));
  }
  else {
    // A new student!
    fp_set_title(t("إضافة حساب طالب جديد"));
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
    "label" => t("تحقق من الأدوار التي يجب أن يمتلكها هذا المستخدم"),
    "type" => "checkboxes",
    "options" => $options,
    "value" => $default_values,
  );
  */  


  // Let's present the form elements to allow some basic editing of this user.
  
  // Only if we are making a new student...
  if ($student_cwid == "new") {
    $form["new_student_cwid"] = array(
      "label" => t(": أدخل رمز مستخدم جديد, مميز للطلبة"),
      "type" => "textfield",
      "size" => 20,
      "required" => TRUE,     
      "description" => t("أدخل هوية لهذا الطالب. قد يكون هو نفسه عضو في هيئة التدريس, ولكن قد لا يكون مثل أي طالب متواجد حاليا. عند حفظ القيمة لن تكون قادرا على تعديلها"),
    );
    
    $form["new_user_name"] = array(
      "label" => t(":أدخل اسم مستخدم جديد, مميز لجميع المستخدمين"),
      "type" => "textfield",
      "size" => 20,     
      "required" => TRUE,
      "description" => t(".أدخل اسم مستخدم لهذا المستخدم. هذا ما سيستخدمه المستخدم لتسجيل الدخول. يجب أن يكون مميزا ا لجميع المستخدمين (لا يمكن أن لعضو من هيئة التدريس وطالب الحصول على نفس اسم المستخدم).بعد حفظ هذه القيمة لن تكون قادرًا على تعديلها"),
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
    "label" => t(": أدخل كلمة سر جديدة لهذا المستخدم"),
    "type" => "textfield",
    "size" => 20,
    "required" => ($student_cwid == "new") ? TRUE : FALSE,
    "description" => t(".LDAP فلن تتأثر كلمة مرور LDAP إذا كنت تستخدم وحدة  ,FlightPath إذا قمت بإدخال أي قيمة هنا، فإنه سيتم تغيير كلمة مرور المستخدم في "),    
  );

  $form["email"] = array(
    "label" => t(": البريد الإلكتروني"),
    "type" => "textfield",
    "value" => $cur["email"],
  );  

  $form["f_name"] = array(
    "label" => t(": الاسم الأول"),
    "type" => "textfield",
    "value" => $cur["f_name"],
  );  

  $form["l_name"] = array(
    "label" => t(": الاسم الأخير"),
    "type" => "textfield",
    "value" => $cur["l_name"],
  );  

  $form["is_disabled"] = array(
    "label" => t(":معطل"),
    "type" => "textfield",
    "value" => $cur["is_disabled"],
    "size" => 5,
    "description" => t("ولن يتمكن من تسجيل الدخول أو البحث عنه. من الآمن تعطيل أحد المستخدمين بدلاً من حذفه FlightPath أدخل 1 أو 0 فقط (رقم واحد لـ نعم ، أو الرقم صفر لـ لا). هذا الإعداد يعني أنه سيتم تجاهل المستخدم من قبل "),
  );  
  
  
  // Unique to students...
  $form["cumulative_hours"] = array(
    "label" => t(":الساعات التراكمية"),
    "type" => "textfield",
    "value" => $cur["cumulative_hours"],
    "size" => 5,
    "description" => t("لحسابها بدلا من القراءة من قاعدة البيانات، سيتم تجاهل كل ما تقوم بإدخاله هنا, وبدلا من ذلك سيتم حساب هذه القيمة عن تحميل الطالب FlightPath كم عدد الساعات التي استحقها الطالب؟ ملاحظة: إذا تم تعيين "),
  );  
    
  $form["gpa"] = array(
    "label" => t(":المعدل التراكمي"),
    "type" => "textfield",
    "value" => $cur["gpa"],
    "size" => 5,
    "description" => t(".لحسابه بدلا من القراء من قاعدة البيانات، سيتم تجاهل كل ما تقوم بإدخاله هنا, وبدلا من ذلك سيتم حسبا هذه القيمة عند تحميل الطالبFlightPath ما هو المعدل التراكمي للطالب؟ ملاحظة: إذا تم تعيين "),    
  );  
    
  $form["rank_code"] = array(
    "label" => t(": التصنيف أو المرتبة"),
    "type" => "textfield",
    "value" => $cur["rank_code"],
    "size" => 5,
    "description" => t(".للسنة الأولى, السنة الثانية, السنة الثالثة, السنة الرابعة, تخرج GR, SR, JR, SO, FR على سبيل المثال ") . "
                      <br><b>" . t(":مهم") . "</b> " . t(".FlightPath إذا لم تقم بذلك فلن تتمكن من البحث عن هذا الطالب في, FR يجب عليك إدخال رمز حقل (تصنيف الطلاب المسموح بهم) في نموذج (إعدادات النظام). على سبيل المثال ") . "<br>&nbsp; &nbsp; &nbsp;" . t(":القيمة الحالية لرتب الطلاب المسموح بها") . " 
                      <i>" . variable_get("allowed_student_ranks", "") . "</i>",
  );  

  $form["major_code"] = array(
    "label" => t(": رمز التخصص"),
    "type" => "textfield",
    "value" => $cur["major_code"],
    "size" => 10,
  );  
    
  $form["catalog_year"] = array(
    "label" => t(": كتالوج العام"),
    "type" => "textfield",
    "value" => $cur["catalog_year"],
    "size" => 10,
    "description" => t(".يتم استخدام السنة الرائدة فقط. على سبيل المثال ، بالنسبة إلى كتالوج العام 2008-2009 ، ما عليك سوى إدخال 2008"),
  );  

  $form["is_active"] = array(
    "label" => t(": نشط"),
    "type" => "textfield",
    "value" => $cur["is_active"],
    "size" => 5,
    "description" => t(".أدخل 1 أو 0 فقط (رقم واحد لـ نعم ، أو الرقم صفر لـ لا). يعني هذا الإعداد أن الطالب لن يظهر في عمليات البحث تلقائيًا، ما لم يحدد المرشد الأكاديمي للبحث عن الطلاب غير النشطين"),
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
                        " . t("حذف هذا الطالب؟") . " <input type='button' value='X'
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
      form_error("new_password", t(" الرجاء إدخال كلمة سر لا تقل عن 5 رموز"));
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
      form_error("new_student_cwid", t(".رمز المستخدم الذي أدخلته ليس رقميا. يجب أن يحتوي على أرقام فقط. يرجى اختيار رمز مستخدم مختلف"));
      return;
    }
    */

    // Check that username is at least 4 characters
    if (strlen($new_user_name) < 4) {
      form_error("new_user_name", t(".اسم المستخدم الذي أدخلته قصير جدًا. يجب أن يكون على الأقل 4 أحرف. يرجى تحديد اسم مستخدم مختلف"));
      return;
      
    }

    
    // Check cwid isn't already in use.
    $test = db_result(db_query("SELECT cwid FROM users WHERE cwid = '?' AND is_student = '1'", $new_cwid));    
    if ($test == $new_cwid) {
      form_error("new_student_cwid", t("رمز المستخدم الذي أدخلته مستخدم حاليا, الرجاء إدخال رمز مستخدم آخر"));
      return;
    }
   
    // Check user_name isn't already in use.
    $test = db_result(db_query("SELECT user_name FROM users WHERE user_name = '?' ", $new_user_name));    
    if ($test == $new_user_name) {
      form_error("new_user_name", t("اسم المستخدم الذي أدخلته مستخدم حاليا, الرجاء إدخال اسم مستخدم آخر"));
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
    
    fp_add_message(t("تم حذف المستخدم."));
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
                                  
    fp_add_message(t("تم إضافة المستخدم بنجاج"));
    fp_goto("admin/users/edit-student-user", "student_cwid=" . $values["new_student_cwid"]);
  }
  
  
      
  fp_add_message(t("تم تحديث المستخدم بنجاح"));
  
  
}

