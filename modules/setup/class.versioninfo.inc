<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage setup
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */


  /**
   * Node for keeping track of moduleversions.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package achievo
   * @subpackage setup
   */
  class versioninfo extends atkNode
  {
    function versioninfo()
    {
      $this->atkNode("versioninfo", NF_READONLY);

      $this->add(new atkAttribute("module" , AF_PRIMARY, 50));
      $this->add(new atkAttribute("version", 0, 15));

      $this->setTable("versioninfo");
    }
  }

?>