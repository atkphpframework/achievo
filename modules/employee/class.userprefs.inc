<?php

useattrib("atkdummyattribute");
useattrib("atknumberattribute");
useattrib("atkpasswordattribute");
useattrib("atklistattribute");
useattrib('employee.adminlangselectattribute');

class userprefs extends atkNode
{
  function userprefs()
  {
    $this->atkNode("userprefs",NF_NO_ADD|NF_NO_DELETE|NF_TRACK_CHANGES);

    $this->add(new atkNumberAttribute("id", AF_PRIMARY|AF_HIDE));
    $this->add(new atkAttribute("userid", AF_HIDE));
    if (atkconfig("authentication")=="db")
    {
      $this->add(new atkDummyAttribute("passwordexplanation", atktext("password_leaveblankifunchanged"), AF_HIDE_ADD|AF_HIDE_LIST));
      $this->add(new atkPasswordAttribute("password",AF_OBLIGATORY));
    }
    $themes = $this->getThemes();
    $this->add(new atkListAttribute("theme",$themes,$themes,AF_HIDE_ADD|AF_NO_TRANSLATION));
    $this->add(new adminLangSelectAttribute('lng',AF_OBLIGATORY));

    $this->setTable("person", "person");
  }

  function descriptor_def()
  {
    return "[userid]";
  }

  function getThemes()
  {
    // Initialize the themelist
    $themelist = array();

    // First look in the themes directory, then look in the atk backend directory
    foreach (array("themes", "atk/themes") as $themedir)
    {
      $handle = opendir($themedir);
      while ($file = readdir($handle))
      {
        if (!in_array($file, array(".", "..", "CVS")) && file_exists($themedir."/".$file."/themedef.inc"))
        {
          $themelist[]= $file;
        }
      }
      closedir($handle);
    }

    // Remove duplicae entries and sort the themelist alphabetically
    $themelist = array_unique($themelist);
    sort($themelist);

    // Return array with themes
    return $themelist;
  }

  function postUpdate($record)
  {
    global $g_user, $g_sessionManager, $g_securityManager, $ATK_VARS;

    if($record["id"]==$g_user["id"])
    {
      // If the theme was updated, we must change it in the session (or you won't
      // see the update.
      $g_sessionManager->globalVar("atkTheme", $record["theme"], true);

      // Also, if we edited userpreferences, then the current record
      // should be stored in the session as user record, so Achievo
      // will immediately start to use the new settings.
      $g_securityManager->reloadUser();

      // Also, we must refresh the screen if the theme or language is changed
      if (($record["theme"] != $record["atkorgrec"]["theme"]) || ($record["lng"] != $record["atkorgrec"]["lng"]))
      {
        $reloadurl = session_url(sprintf("index.php?atknodetype=%s&atkaction=edit&atkselector=%s.%s=%s",
                              $this->atknodetype(),
                              $this->m_table,
                              $this->primaryKeyField(),
                              $ATK_VARS[$this->primaryKeyField()]), SESSION_NEW);
        echo '<script type="text/javascript">';
        echo "top.window.location='$reloadurl';";
        echo '</script>';
        exit();
      }
      else
      {
        return true;
      }
    }
  }
}
?>
