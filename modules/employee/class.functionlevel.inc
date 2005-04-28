<?php

  useattrib("atktextattribute");

  class functionlevel extends atkNode 
  {
    function functionlevel()
    {
      $this->atkNode("functionlevel");
      $this->add(new atkAttribute("id",AF_AUTOKEY));
      $this->add(new atkAttribute("name",AF_OBLIGATORY|AF_SEARCHABLE, 50)); 
      $this->add(new atkTextAttribute("description", 0, AF_HIDE_LIST));
      
      $this->setTable("functionlevel","functionlevel");
      $this->setOrder("name");
    }
    
    function descriptor_def()
    {
      return "[name]";
    }
  }

?>