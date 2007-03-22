<?php
  /**
   * Unittest and webtest file
   *
   * Runs all unittests and webtests within the achievo folder and it's
   * subfolders. In advance to using this file, the config variables
   * whithin this file must be adjusted.
   *
   * @package achievo
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html  GNU General Public License 2
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * @internal includes 
   */
  $config_atkroot = "./";
  include_once("atk.inc");

  // Require ATK authentication if not running in text mode
  if(php_sapi_name() != "cli")
  {
    atksession();
    atksecure();
  }

  // Set the maximum execution time of all tests together
  set_time_limit(atkconfig("test_maxexecutiontime"));

  // Include the Achievo extended webtestcase (features loginAchievo function)
  atkimport("test.achievowebtestcase");

  // Let the atktestsuite run all test files having a name starting with "class.test_"
  $suite = &atknew("atk.test.atktestsuite");
  $suite->run((php_sapi_name() != "cli") ? "html" : "text", atkArrayNvl($_REQUEST, "atkmodule"));

?>