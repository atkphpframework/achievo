<?php
function getThemes() 
{      
  $s = array();
  
  // First look in the themes directory      
  $handle=opendir("themes");
  while ($file = readdir($handle)) 
    if ($file != "." && $file != ".." && $file != "CVS")  $s[]= $file;
  closedir($handle); 
    
  // Then look in the atk backend directory
  $handle=opendir("atk/themes");
  while ($file = readdir($handle)) 
    if ($file != "." && $file != ".." && $file != "CVS") $s[]= $file;
  closedir($handle); 
    
  sort($s);
    
  // return array with themes
  return $s;
}
    
class userprefs extends atkNode
{
  function userprefs()
  {
    $this->atkNode("userprefs",NF_NO_ADD|NF_NO_DELETE); 

    $this->addAttribute(new atkAttribute("userid",AF_PRIMARY|AF_HIDE));
    if (atkconfig("authentication")=="db")
    {
      $this->addAttribute(new atkDummyAttribute("passwordexplanation", text("password_leaveblankifunchanged"), AF_HIDE_ADD|AF_HIDE_LIST));
      $this->addAttribute(new atkPasswordAttribute("password",AF_OBLIGATORY));
    }
    $this->addAttribute(new atkListAttribute("theme",getThemes(),getThemes(),AF_HIDE_ADD));      
    $this->setTable("employee");      
  }

  function descriptor_def()
  {
    return "";
  }
  
  function postUpdate($record)
  {
    global $g_user;
    global $atknoclose;
    global $g_layout;
    global $g_theme;

    // The new session management disables the saveandclose button, so
    // the next commented lines of code no longer worked.
    //if($atknoclose=="")
    //{
      if($record["userid"]==$g_user["name"])
      {
        echo '<script language="javascript">';
        echo 'parent.location = "index.php"';
        echo '</script>';
        exit;
      }
    //}
    //else
    //{
//      $g_theme["Name"] = $record["theme"];
      //$g_layout->initTheme();
      //$g_layout->import_layout();
    //}
  }
}   
?>