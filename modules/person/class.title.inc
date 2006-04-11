<?php

useattrib("atknumberattribute");
 /**
  * The node class for title.
  */
 class title extends atkNode
 {
   function title()
   {
     $this->atkNode("title");
     $this->add(new atkNumberAttribute("id", AF_AUTOKEY));
     
     $this->add(new atkAttribute("title", 0, 25));
     
     $this->setTable("title");
     $this->setSecurityAlias("organization.contact");
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
   
   /**
    * Return record descriptor (for use in relations etc.)
    * @return String A template containing fieldnames between brackets.
    */
   function descriptor_def()
   {
     // @todo: replace this with proper descriptor fields.
     return "[title]";
   }
 }

?>
