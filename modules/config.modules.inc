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
 */

/**
 * Core modules that can't be disabeld
 */
module("utils");
  module("email_template");
  module("setup");
  module("employee");
  module("person");
  module("timereg");
  module("project");
  module("organization");
  module("search");
  module("pim");
  module("graph");

/**
 * Core modules that can be turned off by commenting them
 */
  module("notes");
  module("scheduler");
  module("todo");
  module("reports");
  module("quotation");
  module("docmanager");
  module("crm");

/**
 * Set custom modules below here
 */
