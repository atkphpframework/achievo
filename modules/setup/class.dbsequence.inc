<?php
 
  useattrib("atknumberattribute");

  class dbsequence extends atkNode
  {
    function dbsequence()
    {
      $this->atkNode("dbsequence", NF_READONLY);
      
      $this->add(new atkAttribute("seq_name", AF_PRIMARY, 40));
      $this->add(new atkNumberAttribute("nextid", AF_OBLIGATORY));
     
      $this->setTable("db_sequence");
    }
  }
 
?>