Oracle API
====================
Richard Peacock
richard@richardpeacock.com
getflightpath.com


This module is meant to be a helper module for other developers.  It provides
basic functions for interacting with Oracle, which I feel like makes it easier
to work with than the standard "oci" functions.

--------------
REQUIREMENTS
--------------
You need to have the OCI8 extension already compiled into your PHP installation.

Chances are this has already been done.  But if not, you should review this page:
http://php.net/manual/en/oci8.installation.php


---------------
USE
---------------
If you are building a custom module which needs to make use of an Oracle database,
you would have in its .info file:
  requires = oracleapi

You should then visit the module's settings page and set up the default connection
(not required, but recommended)

Then, when you need to make an oracle query, it should look like this:

  oracleapi_connect();
  $result = oracleapi_query("SELECT * FROM table WHERE name = '?'", $name);

(notice you can use replacement patterns just like db_query)

You can then iterate through the $result like this:

  while ($row = oracleapi_fetch_array($result)) {
    // $row["name"] .....
  }

Note that when using the default connection, it will automatically be closed
when the page finishes loading.  But, if you are handling all the connections
yourself, you should do this:
$con = oracleapi_connect();
// ... //
oracleapi_code($con);

If you wish for your script to die on errors (as in a nightly routine), set this
GLOBALS variable in your script:
  $GLOBALS["fp_oracleapi_die_oracle_errors"] = TRUE;
  
  

------------------
FINE PRINT
------------------
This module is NOT provided or supported by the Oracle company.