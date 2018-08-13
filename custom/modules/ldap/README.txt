LDAP module for FlightPath
==========================
by Richard Peacock
(richardpeacock.com, getflightpath.com)


This module is designed to let you log in to FlightPath
using LDAP credentials, instead of needing local users in your FlightPath directory.

If it is a user's first time to log in, then a dummy local user will be created
in the local users table, in order to not break queries which assume those
tables are populated.

------------
Installation
------------

- Unpack the zip file under your /custom/ directory,
so that ldap.module is at /custom/ldap/ldap.module

- Visit your Admin/Modules page and enable the module.

- Visit the Admin/LDAP settings page to configure.


------------
Hooks
------------

For convenience to the developer, this module can invoke "hooks", that is,
functions in your own custom module.

- hook_ldap_determine_user_type($type, $ldap_result)
  - This hook allows a developer to have a chance to hook in when the user type is
    being determined, to return either TRUE or FALSE, after examining the raw $ldap_result
    returned from the LDAP server.
  - To use, create a function in your custom module with the name of your module instead
    of "hook".  Ex:  "my_module_ldap_determine_user_type($type, $ldap_result) {} "