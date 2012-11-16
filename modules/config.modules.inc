<?php
/**
 *
 * @package achievo
 * @subpackage modules
 *
 */

/**
 * Core modules that must remain enabled
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
 * Core modules that can be disabled (by commenting)
 */
  module("notes");
  module("scheduler");
  module("todo");
  module("reports");
  module("quotation");
  module("docmanager");
  module("crm");

/**
 * Additional modules
 */
