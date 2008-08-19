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
  module("utils");
  module("email_template");
  module("setup");
  module("employee");
  module("person");
  module("timereg");
  module("project");
  module("organization");
  module("notes");
  module("scheduler");
  module("todo");
  module("reports");
  module("pim");
  module("search");
  module("graph");
  module("quotation");
  module("docmanager");
  module("crm");
?>
