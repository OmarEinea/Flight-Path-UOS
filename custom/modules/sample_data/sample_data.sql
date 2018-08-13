/*
This data will be installed when the sample_data module is enabled and installed.
If you want, you can simply load it yourself into your database.

It will overwrite and/or erase data already in your database, so be sure
to back up your database before proceeding.
*/



-- ----------------------------
-- Records of courses
-- ----------------------------

TRUNCATE TABLE `courses`;

INSERT INTO `courses` VALUES ('1', '2319757', 'ACCT', '101', '2012', '', 'There is no course description available at this time.', '3', '3', '3', '0', 'from sample_data module', '0');
INSERT INTO `courses` VALUES ('2', '4214985', 'ACCT', '102', '2012', '', 'There is no course description available at this time.', '3', '3', '3', '0', 'from sample_data module', '0');
INSERT INTO `courses` VALUES ('3', '8370715', 'ACCT', '204', '2012', '', 'There is no course description available at this time.', '3', '3', '3', '0', 'from sample_data module', '0');
INSERT INTO `courses` VALUES ('4', '5867975', 'ACCT', '207', '2012', '', 'There is no course description available at this time.', '1', '1', '1', '0', 'from sample_data module', '0');
INSERT INTO `courses` VALUES ('5', '3787440', 'ACCT', '301', '2012', '', 'There is no course description available at this time.', '3', '3', '3', '0', 'from sample_data module', '0');
INSERT INTO `courses` VALUES ('6', '75913', 'ACCT', '302', '2012', '', 'There is no course description available at this time.', '3', '3', '3', '0', 'from sample_data module', '0');
INSERT INTO `courses` VALUES ('7', '346688', 'ACCT', '345', '2012', '', 'There is no course description available at this time.', '4', '4', '4', '0', 'from sample_data module', '0');
INSERT INTO `courses` VALUES ('8', '4956892', 'ACCT', '401', '2012', '', 'There is no course description available at this time.', '4', '4', '4', '0', 'from sample_data module', '0');
INSERT INTO `courses` VALUES ('9', '8635426', 'ACCT', '455', '2012', '', 'There is no course description available at this time.', '5', '5', '5', '0', 'from sample_data module', '0');



-- ----------------------------
-- Records of degree_requirements
-- ----------------------------

TRUNCATE TABLE `degree_requirements`;

INSERT INTO `degree_requirements` VALUES ('5', '2645063', '0', '0', '', '0', '', '2319757', '', 'm', 'ACCT~101');
INSERT INTO `degree_requirements` VALUES ('6', '2645063', '0', '0', '', '0', '', '4214985', '', 'm', 'ACCT~102');
INSERT INTO `degree_requirements` VALUES ('7', '2645063', '0', '0', '', '0', '', '8370715', '', 'm', 'ACCT~204');
INSERT INTO `degree_requirements` VALUES ('8', '2645063', '0', '0', '', '0', '', '5867975', '', 'm', 'ACCT~207');
INSERT INTO `degree_requirements` VALUES ('9', '2645063', '1', '0', '', '0', '', '3787440', '', 'm', 'ACCT~301');
INSERT INTO `degree_requirements` VALUES ('10', '2645063', '1', '0', '', '0', '', '75913', '', 'm', 'ACCT~302');
INSERT INTO `degree_requirements` VALUES ('11', '2645063', '1', '0', '', '0', '', '8635426', '', 'm', 'ACCT~455');
INSERT INTO `degree_requirements` VALUES ('12', '2645063', '1', '3528994', 'e', '4', '', '0', '', '', '');


-- ----------------------------
-- Records of degrees
-- ----------------------------

TRUNCATE TABLE `degrees`;

INSERT INTO `degrees` VALUES ('1', '2645063', 'ACCT', 'BA', '', 'Accounting', '', ',,', '2012', '0');




-- ----------------------------
-- Records of draft courses
-- ----------------------------

TRUNCATE TABLE `draft_courses`;

INSERT INTO `draft_courses` VALUES ('1', '2319757', 'ACCT', '101', '2012', '', 'There is no course description available at this time.', '3', '3', '3', '0', 'from sample_data module', '0');
INSERT INTO `draft_courses` VALUES ('2', '4214985', 'ACCT', '102', '2012', '', 'There is no course description available at this time.', '3', '3', '3', '0', 'from sample_data module', '0');
INSERT INTO `draft_courses` VALUES ('3', '8370715', 'ACCT', '204', '2012', '', 'There is no course description available at this time.', '3', '3', '3', '0', 'from sample_data module', '0');
INSERT INTO `draft_courses` VALUES ('4', '5867975', 'ACCT', '207', '2012', '', 'There is no course description available at this time.', '1', '1', '1', '0', 'from sample_data module', '0');
INSERT INTO `draft_courses` VALUES ('5', '3787440', 'ACCT', '301', '2012', '', 'There is no course description available at this time.', '3', '3', '3', '0', 'from sample_data module', '0');
INSERT INTO `draft_courses` VALUES ('6', '75913', 'ACCT', '302', '2012', '', 'There is no course description available at this time.', '3', '3', '3', '0', 'from sample_data module', '0');
INSERT INTO `draft_courses` VALUES ('7', '346688', 'ACCT', '345', '2012', '', 'There is no course description available at this time.', '4', '4', '4', '0', 'from sample_data module', '0');
INSERT INTO `draft_courses` VALUES ('8', '4956892', 'ACCT', '401', '2012', '', 'There is no course description available at this time.', '4', '4', '4', '0', 'from sample_data module', '0');
INSERT INTO `draft_courses` VALUES ('9', '8635426', 'ACCT', '455', '2012', '', 'There is no course description available at this time.', '5', '5', '5', '0', 'from sample_data module', '0');



-- ----------------------------
-- Records of draft_degree_requirements
-- ----------------------------

TRUNCATE TABLE `draft_degree_requirements`;

INSERT INTO `draft_degree_requirements` VALUES ('5', '2645063', '0', '0', '', '0', '', '2319757', '', 'm', 'ACCT~101');
INSERT INTO `draft_degree_requirements` VALUES ('6', '2645063', '0', '0', '', '0', '', '4214985', '', 'm', 'ACCT~102');
INSERT INTO `draft_degree_requirements` VALUES ('7', '2645063', '0', '0', '', '0', '', '8370715', '', 'm', 'ACCT~204');
INSERT INTO `draft_degree_requirements` VALUES ('8', '2645063', '0', '0', '', '0', '', '5867975', '', 'm', 'ACCT~207');
INSERT INTO `draft_degree_requirements` VALUES ('9', '2645063', '1', '0', '', '0', '', '3787440', '', 'm', 'ACCT~301');
INSERT INTO `draft_degree_requirements` VALUES ('10', '2645063', '1', '0', '', '0', '', '75913', '', 'm', 'ACCT~302');
INSERT INTO `draft_degree_requirements` VALUES ('11', '2645063', '1', '0', '', '0', '', '8635426', '', 'm', 'ACCT~455');
INSERT INTO `draft_degree_requirements` VALUES ('12', '2645063', '1', '3528994', 'e', '4', '', '0', '', '', '');


-- ----------------------------
-- Records of draft_degrees
-- ----------------------------
TRUNCATE TABLE `draft_degrees`;

INSERT INTO `draft_degrees` VALUES ('1', '2645063', 'ACCT', 'BA', '', 'Accounting', '', ',,', '2012', '0');



-- ----------------------------
-- Records of draft_group_requirements
-- ----------------------------
TRUNCATE TABLE `draft_group_requirements`;

INSERT INTO `draft_group_requirements` VALUES ('1', '3528994', '346688', '', '0', '0', 'ACCT~345');
INSERT INTO `draft_group_requirements` VALUES ('2', '3528994', '4956892', '', '0', '0', 'ACCT~401');

-- ----------------------------
-- Records of draft_groups
-- ----------------------------
TRUNCATE TABLE `draft_groups`;
INSERT INTO `draft_groups` VALUES ('1', '3528994', 'upper_level_acct_electives', 'Upper Level ACCT Electives', '', 'cosc.gif', '2012', '50', '0', 'from sample_data module');



-- ----------------------------
-- Records of faculty
-- ----------------------------
TRUNCATE TABLE `faculty`;

INSERT INTO `faculty` VALUES ('1', '', '', '', '');
INSERT INTO `faculty` VALUES ('111111', 'BA', '', 'Accounting', 'ACCT');



-- ----------------------------
-- Records of group_requirements
-- ----------------------------
TRUNCATE TABLE `group_requirements`;

INSERT INTO `group_requirements` VALUES ('1', '3528994', '346688', '', '0', '0', 'ACCT~345');
INSERT INTO `group_requirements` VALUES ('2', '3528994', '4956892', '', '0', '0', 'ACCT~401');

-- ----------------------------
-- Records of groups
-- ----------------------------
TRUNCATE TABLE `groups`;
INSERT INTO `groups` VALUES ('1', '3528994', 'upper_level_acct_electives', 'Upper Level ACCT Electives', '', 'cosc.gif', '2012', '50', '0', 'from sample_data module');


-- ----------------------------
-- Records of role_permissions
-- ----------------------------
TRUNCATE TABLE `role_permissions`;

INSERT INTO `role_permissions` VALUES ('1', '2', 'access_logged_in_content');
INSERT INTO `role_permissions` VALUES ('2', '3', 'view_any_advising_session');
INSERT INTO `role_permissions` VALUES ('3', '2', 'view_own_advising_session');
INSERT INTO `role_permissions` VALUES ('4', '3', 'view_own_advising_session');
INSERT INTO `role_permissions` VALUES ('5', '3', 'can_advise_students');
INSERT INTO `role_permissions` VALUES ('6', '3', 'can_substitute');
INSERT INTO `role_permissions` VALUES ('7', '2', 'view_comments');
INSERT INTO `role_permissions` VALUES ('8', '3', 'view_comments');
INSERT INTO `role_permissions` VALUES ('9', '3', 'view_faculty_comments');
INSERT INTO `role_permissions` VALUES ('10', '3', 'can_save_comments');
INSERT INTO `role_permissions` VALUES ('11', '3', 'can_delete_own_comments_3_months');
INSERT INTO `role_permissions` VALUES ('12', '3', 'display_search_subtab');
INSERT INTO `role_permissions` VALUES ('13', '3', 'display_my_advisees_subtab');
INSERT INTO `role_permissions` VALUES ('14', '3', 'display_my_majors_subtab');
INSERT INTO `role_permissions` VALUES ('15', '3', 'display_majors_subtab');


-- ----------------------------
-- Records of roles
-- ----------------------------
TRUNCATE TABLE `roles`;

INSERT INTO `roles` VALUES ('1', 'anonymous user');
INSERT INTO `roles` VALUES ('2', 'authenticated user');
INSERT INTO `roles` VALUES ('3', 'advisor');


-- ----------------------------
-- Records of students
-- ----------------------------
TRUNCATE TABLE `students`;

INSERT INTO `students` VALUES ('222222', '23', '2.8', 'FR', 'ACCT', '2012', '1');


-- ----------------------------
-- Records of subjects
-- ----------------------------
TRUNCATE TABLE `subjects`;

INSERT INTO `subjects` VALUES ('ACCT', '', '');


-- ----------------------------
-- Records of user_roles
-- ----------------------------
TRUNCATE TABLE `user_roles`;
INSERT INTO `user_roles` VALUES ('3', '3');


-- ----------------------------
-- Records of Users
-- ----------------------------
DELETE FROM `users` WHERE user_id > 2;

INSERT INTO `users` VALUES ('3', 'sampadv', '64d0b505180d91840d0d6974c6fef497', '0', '1', 'samp@samp.com', '111111', 'Sample', 'Advisor', '0');
INSERT INTO `users` VALUES ('4', 'sampstu', 'c8c2ea7746b0d622f5172ae2d1f0a4f7', '1', '0', 'sampstu@school.com', '222222', 'Sample', 'Student', '0');


-- ----------------------------
-- Records of variables
-- ----------------------------
TRUNCATE TABLE `variables`;

INSERT INTO `variables` VALUES ('admin_transfer_passcode', 's:11:\"apply_draft\";');
INSERT INTO `variables` VALUES ('advising_term_id', 's:6:\"201360\";');
INSERT INTO `variables` VALUES ('allowed_student_ranks', 's:14:\"FR, JR, SO, SR\";');
INSERT INTO `variables` VALUES ('available_advising_term_ids', 's:6:\"201360\";');
INSERT INTO `variables` VALUES ('b_or_better', 's:4:\"B, A\";');
INSERT INTO `variables` VALUES ('contact_email_address', 's:0:\"\";');
INSERT INTO `variables` VALUES ('current_catalog_year', 's:4:\"2012\";');
INSERT INTO `variables` VALUES ('current_draft_catalog_year', 's:4:\"2012\";');
INSERT INTO `variables` VALUES ('c_or_better', 's:7:\"C, B, A\";');
INSERT INTO `variables` VALUES ('d_or_better', 's:10:\"D, C, B, A\";');
INSERT INTO `variables` VALUES ('earliest_catalog_year', 's:4:\"2012\";');
INSERT INTO `variables` VALUES ('enrolled_grades', 's:31:\"E, AMID, BMID, CMID, DMID, FMID\";');
INSERT INTO `variables` VALUES ('graduate_level_course_num', 's:4:\"5000\";');
INSERT INTO `variables` VALUES ('hiding_grades_message', 's:0:\"\";');
INSERT INTO `variables` VALUES ('ignore_courses_from_hour_counts', 's:0:\"\";');
INSERT INTO `variables` VALUES ('maintenance_mode', 'N;');
INSERT INTO `variables` VALUES ('notify_apply_draft_changes_email_address', 's:0:\"\";');
INSERT INTO `variables` VALUES ('notify_mysql_error_email_address', 's:0:\"\";');
INSERT INTO `variables` VALUES ('not_allowed_student_message', 's:0:\"\";');
INSERT INTO `variables` VALUES ('not_released_grades_terms', 's:0:\"\";');
INSERT INTO `variables` VALUES ('retake_grades', 's:4:\"F, W\";');
INSERT INTO `variables` VALUES ('school_initials', 's:4:\"DEMO\";');
INSERT INTO `variables` VALUES ('subjects', 's:8:\"ACCT~~\r\n\";');
INSERT INTO `variables` VALUES ('term_id_structure', 's:0:\"\";');
INSERT INTO `variables` VALUES ('theme', 's:14:\"themes/classic\";');

