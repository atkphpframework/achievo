<?php

class employee extends atkNode
{
  function employee()
  {
    global $ATK_VARS,$g_user;
    $this->atkNode("employee",NF_EDITAFTERADD);
    $this->addAttribute(new atkAttribute("userid" ,AF_PRIMARY|AF_UNIQUE|AF_SEARCHABLE));
    $this->addAttribute(new atkAttribute("name"   ,AF_OBLIGATORY|AF_SEARCHABLE));

    $this->addAttribute(new atkAttribute("email"  ,AF_HIDE_ADD));
    if (atkconfig("authentication")=="db")
    {
      $this->addAttribute(new atkDummyAttribute("passwordexplanation", text("password_leaveblankifunchanged"), AF_HIDE_ADD|AF_HIDE_LIST));
      $this->addAttribute(new atkPasswordAttribute("password", AF_HIDE_ADD|AF_HIDE_LIST|AF_PASSWORD_NOVALIDATE));
    }
    $this->addAttribute(new atkManyToOneRelation("entity", "profile",AF_HIDE_ADD));
    // you can't be your own supervisor neither can a nonactive employee be a superviros
 
    $preFilter = new atkManyToOneRelation("supervisor","employee",AF_HIDE_ADD|AF_HIDE_LIST);

    $preFilter->m_destinationFilter = "userid !='[userid]' && status ='active'";
      
    $this->addAttribute($preFilter,AF_HIDE_ADD);
    // If we are in employee administration mode, we show all users. Otherwise, we only show active users.      
    if ($ATK_VARS["atknodetype"]=="employee"&& in_array($ATK_VARS["atkaction"],array("admin","add","update","edit")))
    {        
      $this->addAttribute(new atkListAttribute("status", array("active","nonactive"), array(), AF_HIDE_ADD));
    }
    else
    {
      $this->addAttribute(new atkListAttribute("status", array("active","nonactive"), array(), AF_HIDE));
      $this->addFilter("status","active");        
    }
    $this->addAttribute(new atkOneToManyRelation("contract","usercontracts","userid",AF_HIDE_ADD|AF_HIDE_LIST));
    
    $this->setTable("employee","employee");
    $this->setOrder("name");
    $this->setIndex("name");
    
    atkdebug("employee::employee()");    
  }

  function initial_values()
  {
    return array("status"=>"active");
  }

  function email_display($record)
  {
    $email = $record["email"];
    if (strlen($email)>0) return '<a href="mailto:'.$email.'">'.$email.'</a>';
    return "";
  }

  function descriptor_def()
  {
    return "[name] ([userid])";
  }

  function action_stats()
  {
    include_once 'employeestats.inc';
  }
}
?>
