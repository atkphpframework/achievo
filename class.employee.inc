<?php

userelation("atkmanytoonerelation");
userelation("atkonetomanyrelation");
useattrib("atklistattribute");
useattrib("atkdummyattribute");
useattrib("atkpasswordattribute");

class useridAttribute extends atkAttribute
{
  function useridAttribute($name, $flags=0)
  {
    /* base class constructor */
    $this->atkAttribute($name, $flags);
  }
  
  function value2db($rec)
  {
    return escapeSQL(str_replace(" ", "_", strtolower($rec["userid"])));
  }
}

class employee extends atkNode
{
  function employee()
  {
    global $ATK_VARS,$g_user;

    $this->atkNode("employee",NF_EDITAFTERADD|NF_TRACK_CHANGES);
    $this->add(new useridAttribute("userid" ,AF_PRIMARY|AF_UNIQUE|AF_SEARCHABLE));
    $this->add(new atkAttribute("name"   ,AF_OBLIGATORY|AF_SEARCHABLE));

    $this->add(new atkAttribute("email"  ,AF_HIDE_ADD));

    $this->add(new atkManyToOneRelation("entity", "profile",AF_HIDE_ADD|AF_RELATION_AUTOLINK));
    // you can't be your own supervisor neither can a nonactive employee be a supervisor
 
    $preFilter = new atkManyToOneRelation("supervisor","employee",AF_HIDE_ADD|AF_HIDE_LIST|AF_RELATION_AUTOLINK);

    $preFilter->m_destinationFilter = "userid !='[userid]' AND status ='active'";
      
    $this->add($preFilter);
    // If we are in employee administration mode, we show all users. Otherwise, we only show active users.      
    if ($ATK_VARS["atknodetype"]=="employee"&& in_array($ATK_VARS["atkaction"],array("admin","add","update","edit")))
    {        
      $this->add(new atkListAttribute("status", array("active","nonactive"), array(), AF_HIDE_ADD));
    }
    else
    {
      $this->add(new atkListAttribute("status", array("active","nonactive"), array(), AF_HIDE));
      $this->addFilter("status","active");        
    }
    $this->addAttribute(new atkOneToManyRelation("contract","usercontracts","userid",AF_HIDE_ADD|AF_HIDE_LIST));
    
    if (atkconfig("authentication")=="db")
    {
      $this->add(new atkDummyAttribute("passwordexplanation", text("password_leaveblankifunchanged"), AF_HIDE_ADD|AF_HIDE_LIST|AF_HIDE_SEARCH));
      $this->add(new atkPasswordAttribute("password", AF_HIDE_ADD|AF_HIDE_LIST|AF_PASSWORD_NOVALIDATE));
    }
    
    $this->setTable("employee","employee");
    $this->setOrder("name");
    $this->setIndex("name");
    
    atkdebug("employee::employee()");    
  }

  function initial_values()
  {
    return array("status"=>"active");
  }
  
  function postUpdate($rec)
  {
    global $g_db;
    $orguser = $rec["atkorgrec"]["userid"];
    $newuser = $rec["userid"];
    if ($newuser!=$orguser)
    {
      atkdebug("Userid changed, updating tables");
      
      // Userid is used as a referential key in several tables. We must update
      // those here. (This is one of those things that makes you wish MySQL had
      // good referential integrity support and commit/rollback.)
      $g_db->query("UPDATE hours SET userid = '$newuser' WHERE userid = '$orguser'");
      $g_db->query("UPDATE hours_lock SET userid = '$newuser' WHERE userid = '$orguser'");
      $g_db->query("UPDATE project SET coordinator = '$newuser' WHERE coordinator = '$orguser'");
      $g_db->query("UPDATE project_notes SET owner = '$newuser' WHERE owner = '$orguser'");
      $g_db->query("UPDATE todo SET owner = '$newuser' WHERE owner = '$orguser'");
      $g_db->query("UPDATE todo SET assigned_to = '$newuser' WHERE assigned_to = '$orguser'");
      $g_db->query("UPDATE usercontract SET userid = '$newuser' WHERE userid = '$orguser'");
      
    }
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