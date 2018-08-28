<?php

class AdvisingScreen extends _AdvisingScreen
{
  
/**
	 * This is used by lots of other functions to display a course on the screen.
	 * It will show the course, the hours, the grade, and quality points, as well
	 * as any necessary icons next to it.
	 * - overriding on 7 August 2017 to be able to display the course titles
	 * @param Course $course
	 * @param string $icon_filename
	 * @param string $title_text
	 * @param bool $js_toggle_and_save
	 *         - If set to TRUE, when the checkbox next to this course is clicked,
	 *           the page will be submitted and a draft will be saved.
	 * 
	 * @param bool $bool_display_check
	 *         - If set to FALSE, no checkbox will be displayed for this course row.
	 * 
	 * @param bool $bool_add_footnote
	 * @param bool $bool_add_asterisk_to_transfers
	 *
	 * @return string
	 */
	function draw_course_row(Course $course, $icon_filename = "", $title_text = "", $js_toggle_and_save = false, $bool_display_check = true, $bool_add_footnote = true, $bool_add_asterisk_to_transfers = false)
	{ 
		//CODE-ADD-START -20170807 the HTML course list table column width change in Advising Screen 
		//$this->width_array = Array("1%", "1%","0%", "0%", "69%", "10%", "10%", "9%");
		//$this->popup_width_array = Array("1%", "1%", "1%", "15%", "42%", "15%", "15%", "10%");
		//CODE-ADD-END
		
		// Display a course itself...
		$pC = "";
		$w1_1 = $this->width_array[0];
		$w1_2 = $this->width_array[1];
		$w1_3 = $this->width_array[2];
		$w2 = $this->width_array[3];
		$w3 = $this->width_array[4];
		$w4 = $this->width_array[5];
		$w5 = $this->width_array[6];
		$w6 = $this->width_array[7];

		$img_path = fp_theme_location() . "/images";
		
		// The current term we are advising for.
		$advising_term_id = $GLOBALS["fp_advising"]["advising_term_id"];
		
    if (!$advising_term_id) {      
      $advising_term_id = 0;
    }

        
		$course->assign_display_status();
		// If the course has already been advised in a different semester,
		// we should set the advising_term_id to that and disable unchecking.
		if ($course->advised_term_id*1 > 0 && $course->bool_advised_to_take == true && $course->advised_term_id != $advising_term_id)
		{
			$course->display_status = "disabled";
			$advising_term_id = $course->advised_term_id;
		}


		if ($course->subject_id == "")
		{
			$course->load_descriptive_data();
		}


		$subject_id = $course->subject_id;
		$course_num = $course->course_num; 


		$o_subject_id = $subject_id;
		$o_course_num = $course_num;

		$footnote = "";
		$ast = "";
		// Is this actually a transfer course?  If so, display its
		// original subject_id and course_num.
		if ($course->bool_transfer == true)
		{
			$subject_id = $course->course_transfer->subject_id;
			$course_num = $course->course_transfer->course_num;
			$institution_name = $course->course_transfer->institution_name;

			if ($bool_add_asterisk_to_transfers == true)
			{
				$course->course_transfer->load_descriptive_transfer_data($this->student->student_id);
				if ($course->course_transfer->transfer_eqv_text != "")
				{
					$ast = "*";
					$GLOBALS["advising_course_has_asterisk"] = true;
				}
			}

			// Apply a footnote if it has a local eqv.
			if ($bool_add_footnote == true && $course->course_id > 0)
			{
				$footnote = "";

				$footnote .= "<span class='superscript'>T";
				$fcount = count($this->footnote_array["transfer"]) + 1;
				if ($course->bool_has_been_displayed == true)
				{ // If we've already displayed this course once, and are
					// now showing it again (like in the Transfer Credit list)
					// we do not want to increment the footnote counter.
					$fcount = $course->transfer_footnote;
				}
				$course->transfer_footnote = $fcount;
				$footnote .= "$fcount</span>";
				$this->footnote_array["transfer"][$fcount] = "$o_subject_id $o_course_num ~~ $subject_id $course_num ~~  ~~ $institution_name";
			}
		}


		if ($course->bool_substitution == true )
		{

			if ($course->course_substitution->subject_id == "")
			{ // Reload subject_id, course_num, etc, for the substitution course,
				// which is actually the original requirement.
				if (is_object($course->course_substitution))
				{
					$course->course_substitution->load_descriptive_data();
				} 
				
			}

			$o_subject_id = $course->course_substitution->subject_id;
			$o_course_num = $course->course_substitution->course_num;

			if ($bool_add_footnote == true)
			{
				$footnote = "";
				$footnote .= "<span class='superscript'>S";
				$fcount = count($this->footnote_array["substitution"]) + 1;
				if ($course->bool_has_been_displayed == true)
				{ // If we've already displayed this course once, and are
					// now showing it again (like in the Transfer Credit list)
					// we do not want to increment the footnote counter.
					$fcount = $course->substitution_footnote;
				}
				$course->substitution_footnote = $fcount;
				$footnote .= "$fcount</span>";
				$this->footnote_array["substitution"][$fcount] = "$o_subject_id $o_course_num ~~ $subject_id $course_num ~~ $course->substitution_hours ~~ $course->assigned_to_group_id";
				
			}
		}

		$hours = $course->hours_awarded * 1;

		if ($hours <= 0) {
		  // Some kind of error-- default to catalog hours
			$hours = $course->get_catalog_hours();
		}

		$hours = $hours * 1;  // force numeric, trim extra zeros.

		$var_hour_icon = "&nbsp;";
		
		
		if ($course->has_variable_hours() == true && !$course->bool_taken)
		{
		  // The bool_taken part of this IF statement is because if the course
		  // has been completed, we should only use the hours_awarded.
		  
			$var_hour_icon = "<img src='" . fp_theme_location() . "/images/var_hour.gif'
								title='" . t("trarabic_10430") . "'
								alt='" . t("trarabic_10431") . "'>";
			$hours = $course->get_advised_hours();

		}

		if ($course->bool_ghost_hour == TRUE) {
		  // This course was given a "ghost hour", meaning it is actually
		  // worth 0 hours, not 1, even though it's hours_awarded is currently
		  // set to 1.  So, let's just make the display be 0.
		  $hours = "0";
		}
		
		$grade = $course->grade;

		$dispgrade = $grade;
		// If there is a MID, then this is a midterm grade.
		$dispgrade = str_replace("MID","<span class='superscript'>" . t("trarabic_10432") . "</span>",$dispgrade);

		if (strtoupper($grade) == "E")
		{ // Currently enrolled.  Show no grade.
			$dispgrade = "";
		}

		if ($course->bool_hide_grade)
		{
		  $dispgrade = "--";
		  $this->bool_hiding_grades = true;
		}
		
		$display_status =  $course->display_status;

		if ($display_status == "completed")
		{
			$pts = $this->get_quality_points($grade, $hours);
		}

		$course_id = $course->course_id;
		$semester_num = $course->assigned_to_semester_num;
		$group_id = $course->assigned_to_group_id;
		$random_id = $course->random_id;
		$advised_hours = $course->advised_hours*1;

		$unique_id = $course_id . "_" . $semester_num . "_" . mt_rand(1,9999);
		$hid_name = "advcr_$course_id" . "_$semester_num" . "_$group_id" . "_$advised_hours" . "_$random_id" . "_$advising_term_id" . "_r" . mt_rand(1,9999);
		
		// Due to an interesting bug, the hid_name cannot contain periods.  So, if a course
		// has decimal hours, we need to replace the decimal with a placeholder.
		if (strstr($hid_name, ".")) {
		  $hid_name = str_replace(".", "DoT", $hid_name);
		}
		
		
		
		$hid_value = "";
		$opchecked = "";
		if ($course->bool_advised_to_take == true)
		{
			$hid_value = "true";
			$opchecked = "-check";
		}

		$op_on_click_function = "toggleSelection";
		if ($js_toggle_and_save == true)
		{
			$op_on_click_function = "toggleSelectionAndSave";
		}

		$extra_js_vars = "";
		if ($course->display_status == "disabled")
		{ // Checkbox needs to be disabled because this was advised in another
			// term.
			$op_on_click_function = "toggleDisabledChangeTerm";
			$course->term_id = $course->advised_term_id;
			$extra_js_vars = $course->get_term_description();

		}

		if ($course->display_status == "completed" || $course->display_status == "enrolled")
		{
			$op_on_click_function = "toggleDisabledCompleted";
			$opchecked = "";
			$extra_js_vars = $course->display_status;
		}

		if ($course->display_status == "retake")
		{
			// this course was probably subbed in while the student
			// was still enrolled, and they have since made an F or W.
			// So, disable it.
			$op_on_click_function = "dummyToggleSelection";
			$opchecked = "";
		}


		if ($this->bool_print || $this->bool_blank)
		{
			// If this is print view, disable clicking.
			$op_on_click_function = "dummyToggleSelection";
		}

		if (!user_has_permission("can_advise_students"))
		{
			// This user does not have the abilty to advise,
			// so take away the ability to toggle anything (like
			// we are in print view).
			$op_on_click_function = "dummyToggleSelection";
		}

		$op = "<img src='$img_path/cb_" . $display_status . "$opchecked.gif'
					border='0'
					id='cb_$unique_id'
					onclick='{$op_on_click_function}(\"$unique_id\",\"$display_status\",\"$extra_js_vars\");'
					>";
		$hid = "<input type='hidden' name='$hid_name'
						id='advcr_$unique_id' value='$hid_value'>";

		// Okay, we can't actually serialize a course, as it takes too much space.
		// It was slowing down the page load significantly!  So, I am going
		// to use a function I wrote called to_data_string().

		$data_string = $course->to_data_string();
		$blank_degree_id = "";
		if ($this->bool_blank == true)
		{
			$blank_degree_id = $this->degree_plan->degree_id;
		}

		$js_code = "describeCourse(\"$data_string\",\"$blank_degree_id\");";

		$icon_link = "";

		// If the course has a 'u' in it, it is a 'University Capstone' course.
		if (strstr($course->requirement_type, "u")) {
			$icon_filename = "ucap.gif";
			$title_text = t("trarabic_10433");
		}

		if ($icon_filename != "") {
			$icon_link = "<img src='" . fp_theme_location() . "/images/icons/$icon_filename' width='19' height='19' border='0' alt='$title_text' title='$title_text'>";
		}

		$on_mouse_over = " onmouseover=\"style.backgroundColor='#FFFF99'\"
      				onmouseout=\"style.backgroundColor='white'\" ";

		if (fp_screen_is_mobile()) $on_mouse_over = "";  // Causes problems for some mobile devices.
		
		$hand_class = "hand";

		if ($bool_display_check == false) {
			$op = $hid = "";
		}


		if ($this->bool_print) {
			// In print view, disable all popups and mouseovers.
			$on_mouse_over = "";
			$js_code = "";
			$hand_class = "";
		}


		$pC .= "<tr><td colspan='8'>";


		if ($course->bool_substitution_new_from_split != true || ($course->bool_substitution_new_from_split == true && $course->display_status != "eligible")){

			if ($course_num == ""){
				$course_num = "&nbsp;";
			}

 
			$pC .= "
   		<table border='0' cellpadding='0' width='100%' cellspacing='0' align='left'>
     	<tr height='20' class='$hand_class $display_status'
      		$on_mouse_over title='$title_text'>
      		<td style='width:$w1_1; white-space:nowrap;' align='left'>$op$hid</td>
      		<td style='width:$w1_2; white-space:nowrap;' align='left' onClick='$js_code'>$icon_link</td>
      		<td style='width:$w1_3; white-space:nowrap;' align='left' onClick='$js_code'>&nbsp;$ast</td>
      		<td align='left' style='width:$w2; white-space:nowrap;' class='tenpt underline' onClick='$js_code'>
       				$subject_id</td>
       		<td class='tenpt underline' style='width:$w3; white-space:nowrap;' align='left' 
       			onClick='$js_code'>
        			$course_num$footnote  - Title Course</td>
	       <td class='tenpt underline' style='width:$w4; max-width:36px; white-space:nowrap;' onClick='$js_code'>$hours$var_hour_icon</td>
       	   <td class='tenpt underline'  style='width:$w5; max-width:35px; white-space:nowrap;' onClick='$js_code'>$dispgrade&nbsp;</td>
       	   <td class='tenpt underline' style='width:$w6; max-width:31px; white-space:nowrap;' onClick='$js_code'>$pts&nbsp;</td>
     	</tr>
     	</table>";

		} else {
			// These are the leftover hours from a partial substitution.

			$pC .= "
   		<table border='0' cellpadding='0' width='100%' cellspacing='0' align='left'>
     	<tr height='20' class='hand $display_status'
      		$on_mouse_over title='$title_text'>
      		<td width='$w1_1' align='left'>$op$hid</td>
      		<td width='$w1_2' align='left' onClick='$js_code'>$icon_link</td>
      		<td width='$w1_3' align='left' onClick='$js_code'>&nbsp;</td>
      		<td align='left' class='tenpt underline' onClick='$js_code'
      			colspan='4'>
       				&nbsp; &nbsp; $subject_id &nbsp;
        			$course_num$footnote
	       			&nbsp; ($hours " . t("trarabic_10434") . ")
       	   	</td>
     	</tr>
     	</table>";		

		}

		$pC .= "</td></tr>";


		return $pC;
	}
  
/**
	 * Used to draw the beginning of semester boxes and other boxes, for example
	 * the footnotes.
	 *
	 * @param string $title
	 * @param bool $hideheaders
	 *       - If TRUE, then the course/hrs/grd headers will not be displayed.
	 * 
	 * @param int $table_width
	 *       - The HTML table width, in pixels.  If not set, it will default
	 *         to 300 pixels wide.
	 * 
	 * @return string
	 */
	function draw_box_top($title, $hideheaders=false, $table_width = 300){ 
	  // returns the beginnings of the year tables...

		// Get width values from width_array (supplied by calling function,
		// for example, draw_year_box_top
		$w1_1 = $this->width_array[0];
		$w1_2 = $this->width_array[1];
		$w1_3 = $this->width_array[2];
		$w2 = $this->width_array[3];
		$w3 = $this->width_array[4];
		$w4 = $this->width_array[5];
		$w5 = $this->width_array[6];
		$w6 = $this->width_array[7];

		if ($this->bool_popup == true)
		{
			$w1_1 = $this->popup_width_array[0];
			$w1_2 = $this->popup_width_array[1];
			$w1_3 = $this->popup_width_array[2];
			$w2 = $this->popup_width_array[3];
			$w3 = $this->popup_width_array[4];
			$w4 = $this->popup_width_array[5];
			$w5 = $this->popup_width_array[6];
			$w6 = $this->popup_width_array[7];
		}


		$headers = array();
		if ($hideheaders != true)
		{
			$headers[0] = t("trarabic_10435");
			$headers[1] = t("trarabic_10436");
			$headers[2] = t("trarabic_10437");
			$headers[3] = t("trarabic_10438");
		}


		$rtn = "
		   <table border='0' width='$table_width' cellpadding='0' cellspacing='0' class='fp-box-top'>
   			<tr>
    		<td colspan='8' class='blueTitle' align='center' valign='top'>
    				";
		$rtn .= fp_render_curved_line($title);

		$rtn .= "
    		</td>
   			</tr>
   					";
		if (!$hide_headers)
		{
			$rtn .= "
   			<tr height='20'>

    			<td width='$w1_1' align='left'>
     			&nbsp;
    			</td>

    			<td width='$w1_2' align='left'>
     			&nbsp;
    			</td>

    			<td width='$w1_3' align='left'>
     			&nbsp;
    			</td>
    
        		<td align='left' width='$w2'>
     				<font size='2'><b>$headers[0]</b></font>
	    		</td>

    			<td width='$w3' align='left'>&nbsp;</td>
    			<td width='$w4'>
     				<font size='2'><b>$headers[1]</b></font>
    			</td>
    			<td width='$w5'>
     				<font size='2'><b>$headers[2]</b></font>
    			</td>
    			<td width='$w6'>
     				<font size='2'><b>$headers[3]</b></font>
    			</td>
   			</tr>
   				";
		}
		return $rtn;

	} // draw_year_box_top


}