-- phpMyAdmin SQL Dump
-- version 4.7.9
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1:3306
-- Generation Time: Aug 27, 2018 at 09:04 PM
-- Server version: 5.7.21
-- PHP Version: 5.6.35

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `fp`
--

-- --------------------------------------------------------

--
-- Table structure for table `variables`
--

DROP TABLE IF EXISTS `variables`;
CREATE TABLE IF NOT EXISTS `variables` (
  `name` varchar(255) NOT NULL,
  `value` text,
  PRIMARY KEY (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `variables`
--

INSERT INTO `variables` (`name`, `value`) VALUES
('#submit_handlers', 'N;'),
('#validate_handlers', 'N;'),
('admin_transfer_passcode', 's:11:\"apply_draft\";'),
('advising_term_id', 's:6:\"201730\";'),
('allowed_student_ranks', 's:53:\"C1,C2,C3,DR,FY,GR,IE,M1,M2,M3,M4,M5,Y1,Y2,Y3,Y4,Y5,Y6\";'),
('autocapitalize_course_titles', 's:3:\"yes\";'),
('autocapitalize_institution_names', 's:3:\"yes\";'),
('available_advising_term_ids', 's:225:\"201110, 201115, 201120, 201130, 201210, 201215, 201220, 201230, 201310, 201315, 201320, 201330, 201410, 201415, 201420, 201430,  201510, 201515, 201520, 201530, 201610, 201615, 201620, 201630, 201710, 201715, 201720, 201730, \";'),
('banner_integration_enrolled_grade', 's:1:\"E\";'),
('banner_integration_exclude_historical_courses_code', 's:1:\"E\";'),
('banner_integration_exclude_transfer_courses_code', 's:1:\"E\";'),
('banner_integration_load_student_local_courses', 'N;'),
('banner_integration_routines_import_local_courses_settings', 'a:1:{s:21:\"update_prod_and_draft\";s:21:\"update_prod_and_draft\";}'),
('b_or_better', 's:8:\"B, B+, A\";'),
('calculate_cumulative_hours_and_gpa', 'N;'),
('calculate_cumulative_hours_and_gpa_copy', 'b:1;'),
('clean_urls', 'N;'),
('contact_email_address', 's:22:\"shabih.fatima@live.com\";'),
('course_repeat_policy', 's:28:\"most_recent_exclude_previous\";'),
('course_search_avail_term_headers', 's:0:\"\";'),
('course_search_avail_term_id_suffix_order', 's:8:\"10,20,30\";'),
('course_search_avail_term_mobile_headers', 's:0:\"\";'),
('cron_last_run', 'i:1535354118;'),
('css_js_query_string', 's:6:\"pdjpkk\";'),
('current_catalog_year', 's:4:\"2017\";'),
('current_draft_catalog_year', 's:0:\"\";'),
('c_or_better', 's:15:\"C, C+, B, B+, A\";'),
('developmentals_notice', 's:195:\"According to our records, you are required to complete the course(s) listed above. For some transfer students, your record may not be complete. If you have any questions, please ask your advisor.\";'),
('developmentals_title', 's:26:\"Developmental Requirements\";'),
('disallow_graduate_credits', 's:3:\"yes\";'),
('display_graduate_credits_block', 's:3:\"yes\";'),
('d_or_better', 's:25:\"P, D, D+, C, C+, B, B+, A\";'),
('earliest_catalog_year', 's:4:\"2007\";'),
('enrolled_grades', 's:43:\"RE, RF, RW, E, AMID, BMID, CMID, DMID, FMID\";'),
('enrolled_student_search_conditions', 's:19:\" AND is_active = 1 \";'),
('extra_student_search_conditions', 's:19:\" AND is_active = 1 \";'),
('grade_order', 's:0:\"\";'),
('graduate_credits_block_notice', 's:55:\"These courses may not be used for undergraduate credit.\";'),
('graduate_credits_block_title', 's:16:\"Graduate Credits\";'),
('graduate_level_codes', 's:2:\"GR\";'),
('graduate_level_course_num', 's:4:\"5000\";'),
('hiding_grades_message', 's:0:\"\";'),
('ignore_courses_from_hour_counts', 's:0:\"\";'),
('initial_student_course_sort_policy', 's:5:\"alpha\";'),
('maintenance_mode', 's:25:\"BOOLEAN_FALSE_PLACEHOLDER\";'),
('mark', 'N;'),
('mark1', 'N;'),
('max_watchdog_age', 's:3:\"548\";'),
('minimum_substitutable_grade', 's:1:\"D\";'),
('notify_apply_draft_changes_email_address', 's:0:\"\";'),
('notify_mysql_error_email_address', 's:0:\"\";'),
('not_allowed_student_message', 's:0:\"\";'),
('not_released_grades_terms', 's:0:\"\";'),
('old_allowed_student_ranks', 's:14:\"FR, JR, SO, SR\";'),
('pie_chart_config', 's:227:\"r ~ University Comp\r\nl ~ University Elecs\r\nc ~ College Comp\r\nd ~ College Elecs\r\nm ~ Major Comp\r\nf ~ Major Elecs\r\nh ~ Minor Comp\r\ng ~ Minor Elecs\r\ni ~ Supp Comp\r\nj ~ Supp Elecs\r\ny ~ Conc Comp\r\ne ~ Electives\r\nx ~ Ignored From GPA\";'),
('quality_points_grades', 's:116:\"A ~ 4\r\nB+ ~ 3.5\r\nB ~ 3\r\nC+ ~ 2.5\r\nC ~ 2\r\nD+ ~ 1.5\r\nD ~ 1\r\nF ~ 0\r\nF* ~ 0\r\nFA ~ 0\r\nI ~ 0\r\nP ~ 0\r\nW ~ 0\r\nWF ~ 0\r\nWP ~ 0\";'),
('requirement_types', 's:227:\"r ~ University Comp\r\nl ~ University Elecs\r\nc ~ College Comp\r\nd ~ College Elecs\r\nm ~ Major Comp\r\nf ~ Major Elecs\r\nh ~ Minor Comp\r\ng ~ Minor Elecs\r\ni ~ Supp Comp\r\nj ~ Supp Elecs\r\ny ~ Conc Comp\r\ne ~ Electives\r\nx ~ Ignored From GPA\";'),
('restrict_ghost_subs_to_ghost_hours', 's:3:\"yes\";'),
('restrict_ghost_subs_to_ghost_hours_old', 's:3:\"yes\";'),
('retake_grades', 's:24:\"EX, F, F*, FA, W, WF, WP\";'),
('school_initials', 's:3:\"UoS\";'),
('site_token', 's:32:\"a6f59f5944fe93b25f2e0feb53c4bb23\";'),
('student_search_major_search_cur_year', 'N;'),
('subjects', 's:8:\"ACCT~~\r\n\";'),
('sub_hours_decimals_allowed', 's:1:\"2\";'),
('support_clean_urls', 's:3:\"yes\";'),
('system_execute_php', 's:12:\"echo \"Test\";\";'),
('term_id_structure', 's:1114:\"[2007]10, 2007 Fall\r\n[2007]15, 2007 Full-Year\r\n[2007]20, 2007 Spring\r\n[2007]30, 2007 Summer\r\n[2008]10, 2008 Fall\r\n[2008]15, 2008 Full-Year\r\n[2008]20, 2008 Spring\r\n[2008]30, 2008 Summer\r\n[2009]10, 2009 Fall\r\n[2009]15, 2009 Full-Year\r\n[2009]20, 2009 Spring\r\n[2009]30, 2009 Summer\r\n[2010]10, 2010 Fall\r\n[2010]15, 2010 Full-Year\r\n[2010]20, 2010 Spring\r\n[2010]30, 2010 Summer\r\n[2011]10, 2011 Fall\r\n[2011]15, 2011 Full-Year\r\n[2011]20, 2011 Spring\r\n[2011]30, 2011 Summer\r\n[2012]10, 2012 Fall\r\n[2012]15, 2012 Full-Year\r\n[2012]20, 2012 Spring\r\n[2012]30, 2012 Summer\r\n[2013]10, 2013 Fall\r\n[2013]15, 2013 Full-Year\r\n[2013]20, 2013 Spring\r\n[2013]30, 2013 Summer\r\n[2014]10, 2014 Fall\r\n[2014]15, 2014 Full-Year\r\n[2014]20, 2014 Spring\r\n[2014]30, 2014 Summer\r\n[2015]10, 2015 Fall\r\n[2015]15, 2015 Full-Year\r\n[2015]20, 2015 Spring\r\n[2015]30, 2015 Summer\r\n[2016]10, 2016 Fall\r\n[2016]15, 2016 Full-Year\r\n[2016]20, 2016 Spring\r\n[2016]30, 2016 Summer\r\n[2017]10, 2017 Fall\r\n[2017]15, 2017 Full-Year\r\n[2017]20, 2017 Spring\r\n[2017]30, 2017 Summer\r\n[2018]10, 2018 Fall\r\n[2018]15, 2018 Full-Year\r\n[2018]20, 2018 Spring\r\n[2018]30, 2018 Summer\";'),
('theme', 's:14:\"themes/classic\";'),
('update_status_last_run', 'i:1535354118;'),
('update_status_need_updates_modules', 'N;'),
('update_status_need_updates_release_types', 'N;'),
('withdrew_grades', 's:12:\"W, WF, WP, I\";');
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
