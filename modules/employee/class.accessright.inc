<?php

  class accessright extends atkNode
  {
    function accessright()
    {
      // This is a 'dummy' node which can only be used for retrieval
      // of access information and installing the database tables.
      // It's not intended as an editor for rights. Use the profile
      // editor if you want to edit rights.
      // The readonly flag is set to prevent accidental editing through
      // this class.
      $this->atkNode("accessright", NF_READONLY);
      
      $this->add(new atkAttribute("node", AF_PRIMARY, 100));
      $this->add(new atkAttribute("action", AF_PRIMARY, 25));
      $this->add(new atkAttribute("entity", AF_PRIMARY, 10));
      
      $this->setTable("accessright");
    }
  }

?>