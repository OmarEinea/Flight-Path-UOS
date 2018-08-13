Banner Integration
==================
richard@richardpeacock.com
getflightpath.com


To read an online guide for setting up this module with your school, see:
http://getflightpath.com/node/996

---------------
DESCRIPTION
---------------

This module attempts to bring together several functions and techniques for working
with the Banner system, by Ellucian.

Please note that this module is neither provided nor maintained by Ellucian.

This is the system used by the University of Louisiana at Monroe (the university
which orginally created FlightPath).  As such, we have several functions, routines, etc,
which we use to pull Banner data into FlightPath.


---------------
USE
---------------

It is assumed you will be using this module's routines functions, as well as the "live" pulls
from Banner.

See the sample_routine.php.txt file, for a guide on setting up a nightly routine to bring
in the banner data.  Be sure to read through all of the comments so you understand what this file
is doing, and be sure to change the $SECRET_PASSCODE value to something unique.


---------------
HOOKS
---------------

This module offers the following hooks:
 - hook_banner_integration_get_catalog_year_for_term
    - Lets your custom module override how a student's catalog year is determined, based on
      the "catalog year term" from Banner.
 - hook_banner_integration_routines_transfer_credit_record_alter
    - Allows custom modules to modify the result coming out of Banner, before being processed
      in the routine.  You can also pass back instructions to the routine to skip the record
      entirely, or note that there is no local eqv for this transfer course.
    - Used by ULM, for example, to convert the attn_period from "SPRING 2012" to a proper
      term "201260".



----------------
REQUIREMENTS
----------------

You will need to install the "oracleapi" module, and configure its default connection (in the
admin settings page) to connect to your Banner installation.

