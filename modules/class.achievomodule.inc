<?php

  /**
   * Extension to the standard atkModule, with an extra API
   * specially for some Achievo functions.
   */
  
  class achievoModule extends atkModule
  {
    function achievoModule($name="")
    {
      $this->atkModule($name);
    }
    
    /**
     * Get elements for the Personal Information Manager screen.
     *
     * Modules can override this method to return their own
     * specific items for the PIM. The method should return 
     * an associative array, with the following format:
     * $itemname=>$information
     * Example:
     * return array("hello"=>"Hello World")
     *
     * The item will then be displayed on the pim page, with
     * a localized $txt_pim_hello as title.
     */
    function getPimItems()
    {
      return array();
    }
  }