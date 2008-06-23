<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage person
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */

include_once (moduleDir("person")."/class.person.inc");

/**
 * Select person class
 *
 * @package achievo
 * @subpackage person
 */
class select_person extends person
{
  /**
   * Constructor
   */
  function select_person()
  {
    $this->person("select_person");

    // for employees the nonactive ones must be filtered out.
    $this->addFilter("(person.status <> 'nonactive' OR person.role <> 'employee')");

    $this->remove("role");
    $this->remove("fax");
    $this->add(new atkAttribute("role", AF_SEARCHABLE));
    $this->setSecurityAlias("person.person");
  }

}

?>