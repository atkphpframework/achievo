<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage modules
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */

/**
 * Turn on the default achievo modules, custom modules should be
 * added in the config.inc.php
 */
  module("utils",'',MF_NO_PRELOAD);
  module("email_template",'',MF_NO_PRELOAD);
  module("setup",'',MF_NO_PRELOAD);
  module("employee",'',MF_NO_PRELOAD);
  module("person",'',MF_NO_PRELOAD);
  module("timereg",'',MF_NO_PRELOAD);
  module("project",'',MF_NO_PRELOAD);
  module("organization",'',MF_NO_PRELOAD);
  module("notes",'',MF_NO_PRELOAD);
  module("scheduler",'',MF_NO_PRELOAD);
  module("todo",'',MF_NO_PRELOAD);
  module("reports",'',MF_NO_PRELOAD);
  module("pim",'',MF_NO_PRELOAD);
  module("search",'',MF_NO_PRELOAD);
  module("graph",'',MF_NO_PRELOAD);
  module("quotation",'',MF_NO_PRELOAD);
  module("docmanager",'',MF_NO_PRELOAD);
  module("crm",'',MF_NO_PRELOAD);
?>
