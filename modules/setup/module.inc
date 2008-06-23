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
   * Module file for the menu items and security
   *
   * @package achievo
   * @subpackage setup
   */
  class mod_setup extends achievoModule
  {
    function getMenuItems()
    {
      $this->menuitem("setup","","main",true,700);

      $this->menuitem("support","","main",true,800);
      $this->menuitem("achievo_docs", "http://www.achievo.org/wiki/Achievo", "support");
      $this->menuitem("-", "", "support");
      $this->menuitem("achievo_license", dispatch_url("setup.license","view"),"support");
      $this->menuitem("-", "", "support");
      $this->menuitem("online_support", "http://www.achievo.org/services/", "support");
      $this->menuitem("sponsorship", "http://www.achievo.org/services/sponsor/", "support");
      $this->menuitem("-", "", "support");
      $this->menuitem("achievo_home", "http://www.achievo.org/","support");
    }
  }

?>