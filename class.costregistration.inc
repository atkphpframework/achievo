<?php

include_once "class.projectattrib.inc";  

  class costregistration extends atkNode
  {
    function costregistration()
    {
      $this->atkNode("costregistration"); // node() constructor is *not* called automatically!
     
      $this->addAttribute(new atkAttribute("id", AF_PRIMARY|AF_UNIQUE|AF_AUTO_INCREMENT|AF_HIDE));
      $this->add(new atkAttribute("userid", AF_READONLY));
      	$this->addAttribute(new atkDateAttribute("date","F d Y","d F Y", 0, date("Ymd"),AF_OBLIGATORY));
      $this->addAttribute(new atkProjectAttribute("project_id", AF_OBLIGATORY|AF_SEARCHABLE));
      
      //$this->addAttribute(new atkManytoOneRelation("project_id","project", AF_OBLIGATORY|AF_SEARCHABLE));
      //$this->add(new atkDateAttribute("date", "F d Y","d F Y", 0, date("Ymd"),AF_OBLIGATORY);
      $this->addAttribute(new atkManyToOneRelation("currency","finance.currency"));
      $this->addAttribute(new atkAttribute("value", AF_OBLIGATORY));
      $this->add(new atkListAttribute("Paymethod",array("Cash","Electronic Payment","Creditard"), array("Cash","Electronic Payment","CreditCard"), AF_OBLIGATORY));
      $this->add(new atkAttribute("description", AF_OBLIGATORY));
      
      //$this->add(new atkDummyAttribute("This is a dummy attribute"));
      //$this->add(new atkFileAttribute("File",array("themes","http://localhost/themes")));
      //$this->add(new atkHtmlAttribute("HTML"));
      //$this->add(new atkLimitedTextAttribute("LimitedText"));
      //$this->add(new atkRadioAttribute("Radio",array("Option 1","Option 2","Option 3"),array("option1","option2","option3")));
      //$this->add(new atkMultipleFileAttribute("MultipleFile",array("themes","http://locahost/themes")));
      //$this->add(new atkNumberAttribute("Number"));
      //$this->add(new atkParserAttribute("Parser","Test"));
      //$this->add(new atkPasswordAttribute("Password"));
      //$this->add(new atkTextAttribute("Text",AF_LIST_HIDE));
      //$this->add(new atkTimeAttribute("Time"));
      
      atkdebug("costregistration::costregistration()");
      
      //$this->setOrder("news_id");
      //$this->setFilter("test='test'");
      
      $this->setTable("costregistration");
    }

    function initial_values()
    {
      global $g_user;
      return array("userid"=>strtolower($g_user["name"]));  
    }

    function value_display($rec)
    {
      $value = $rec["value"];
      $value = round($value, 2);
      return "$value";
    }

    function edit_values($record)
    {
       atkdebug("EDIT VALUES");
       return Array("hits"=>100);
    }


    function descriptor_def()
    {
      return "[heading]";
    }
  }

?>