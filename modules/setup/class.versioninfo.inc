<?php

  /**
   * Node for keeping track of moduleversions.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   *
   * @version $Revision$
   *
   * $Id$
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