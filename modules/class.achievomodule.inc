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
     * Modules can override this method to return their own specific items for
     * the PIM. The method should return an associative array, with the 
     * following format:
     *
     * array($itemname=>$information);
     * Example:
     * return array("hello"=>"Hello World")
     *
     * The item will then be displayed on the pim page, with a localized 
     * $txt_pim_hello as title.
     */
    function getPimItems()
    {
      return array();
    }
    
    /**
     * Search the module for a certain expression.
     *
     * Modules can override this method to return their own search results 
     * for the global search feature. The method should return an associative 
     * array, with the following format:
     *
     * array($category=>array(0=>array("title"=>$resulttitle,
     *                                 "url"=>$url)));
     * Example:
     * return array("customers"=>array(0=>array("title"=>"Ibuildings.nl BV",
     *                                          "url"=>$viewcustomerurl)));
     * The searchresultpage will show results per category, and the category
     * will be displayed as $txt_category.
     */
    function search($expression)
    {
      return array();
    }
  }