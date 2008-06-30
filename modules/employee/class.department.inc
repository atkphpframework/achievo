<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage employee
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */
useattrib("atkNumberAttribute");
useattrib("atkTextAttribute");

/**
 * Class for managing departments
 * @package achievo
 * @subpackage employee
 */
class department extends atkNode
{
  /**
   * Constructor
   */
  function department()
  {
    $this->atkNode("department",NF_AUTOSELECT);
    $this->add(new atkNumberAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("name",AF_OBLIGATORY));
    $this->add(new atkTextAttribute("description"));
    $this->setOrder("name");
    $this->setTable("employee_department");
  }

  /**
   * Descriptor template
   *
   * @return string
   */
  function descriptor_def()
  {
    return "[name]";
  }
}

?>