<?php
  define("DEP_INSTALL", 1);
  define("DEP_RUN"    , 2);
  define("DEP_ALL"    , DEP_INSTALL|DEP_RUN);

  /**
   * Extension to the standard atkModule, with an extra API
   * specially for some Achievo functions.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @version $Revision$
   *
   * $Id$
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

    /**
     * Find out on which other modules this module depends.
     *
     * @param $mode Indicates what kind of dependency to check. This can have
     *              the following values:
     *              DEP_RUN     - Return the dependencies that must be met in
     *                            order to run the module.
     *              DEP_INSTALL - Return the dependencies that must be met in
     *                            order to install the module.
     *              DEP_ALL     - Return the list of all dependencies
     *              To explain the reason why this parameter exists, I'll give
     *              an example:
     *              Customers have a onetomany relation with Projects, and
     *              Projects have a manytoone relation on Customer. So, to
     *              run any of the two, both must be installed. So Customer
     *              has a dependency on Project. However, during the
     *              installation process, the Customer table can be installed
     *              before the Project table (no foreign key), so the
     *              dependency is not (yet) necessary. (If we wouldn't do it
     *              like this, you could get cyclical dependencies).
     *
     * @return an array of modulenames this module is dependent on.
     */
    function getDependencies($mode)
    {
      return array();
    }

    function recLinks($recordset, $nodetype)
    {
      $node = &atkGetNode($nodetype);

      $res = array();

      for($i=0, $_i=count($recordset); $i<$_i; $i++)
      {
        $item["title"] = $node->descriptor($recordset[$i]);
        $item["url"] = dispatch_url($nodetype, "view", array("atkselector"=>$node->primaryKey($recordset[$i])));
        $res[] = $item;
      }

      return $res;
    }
  }
?>