<?php

  useattrib("atkcolorpickerattribute");
  useattrib("atknumberattribute");

  /**
   * The node class for mastergantt_colorconfig.
   */
  class mastergantt_colorconfig extends atkNode
  {
    function mastergantt_colorconfig()
    {
      $this->atkNode("mastergantt_colorconfig", NF_ADD_LINK);
      $this->add(new atkNumberAttribute("id", AF_AUTOKEY));
      
      $this->add(new atkNumberAttribute("hours_min"));
      $this->add(new atkNumberAttribute("hours_max"));
      $this->add(new atkColorPickerAttribute("color", ""));
      
      $this->setTable("mastergantt_colorconfig");
      
      
      // We only have a simple 'edit' privilege. This implies add/delete rights.
      $this->addSecurityMap(array("add", "save", "delete"), "edit");
    }
    
    /**
     * Set up default values for this node
     * @return array Associative array where the key is
     *               the attribute name, and value is the
     *               default value.
     */
    function initial_values()
    {    
      return array();
    }
    
    function validate(&$record, $mode, $ignoreList=array())
    {    
      // crosscomparison to determine overlap
      if ($record['hours_min']=="") $record["hours_min"] = 0;
      $max = $record["hours_max"];
      if ($max=="") $max = 9999999; // improbable project maximum for
                                                                    // easier validation (yes, I am lazy) 
                                                                    // - Ivo
      $filter = sprintf('(%d < hours_max OR hours_max IS NULL) AND 
                          (%d > hours_min OR hours_min IS NULL)
                        ',$record['hours_min'],$max); 
      if ($mode=="update") $filter.= sprintF(" AND id<>%d",$record["id"]);
      
      $existing = $this->selectDb($filter, "", "", "", array("id"));
      
      if (count($existing))
      {
        triggerError($record, "hours_max", "error_colorconfig_overlap");
      }
      
      return parent::validate($record, $mode, $ignoreList); 
    }
    
    /**
     * Return record descriptor (for use in relations etc.)
     * @return String A template containing fieldnames between brackets.
     */
    function descriptor_def()
    {
      // @todo: replace this with proper descriptor fields.
      return "[hours_min]-[hours_max]";
    }
    
    function getColor($hours)
    {
      static $s_colors = array();
      if (!count($s_colors))
      {
        $s_colors = $this->selectDb("", "hours_min, hours_max");
      }
      
      foreach($s_colors as $color)
      {
        if (($color["hours_min"]==""|| $hours >= $color["hours_min"])
            && ($color["hours_max"]=="" || $hours < $color["hours_max"]))
        {
          return $color["color"];
        }
      }
      return "#cccccc"; // no matching color found, using neutral gray
    }
  }

?>