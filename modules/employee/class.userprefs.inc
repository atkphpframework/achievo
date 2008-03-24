<?php

useattrib("atkdummyattribute");
useattrib("atknumberattribute");
useattrib("atkpasswordattribute");
useattrib("atklistattribute");
useattrib('employee.adminlangselectattribute');

class userprefs extends atkNode
{
  /**
   * Constructor
   *
   * @todo support more startdays (http://www.pjh2.de/datetime/weeknumber/wnd.php?l=en)
   * @return userprefs
   */
  function userprefs()
  {
    $this->atkNode("userprefs",NF_NO_ADD|NF_NO_DELETE|NF_TRACK_CHANGES);

    $this->add(new atkNumberAttribute("id", AF_PRIMARY|AF_HIDE));
    $this->add(new atkAttribute("userid", AF_HIDE));
    if (atkconfig("authentication")=="db")
    {
      $this->addPasswordAttribute();
    }
    $themes = $this->getThemes();
    $this->add(new atkListAttribute("theme",$themes,$themes,AF_HIDE_ADD));
    $this->add(new adminLangSelectAttribute('lng',AF_OBLIGATORY));

    $this->setTable("person", "person");
  }

  function initial_values()
  {
    return array("lng"=>atkconfig("language"));
  }
  
  function addPasswordAttribute()
  {
    $this->add(new atkDummyAttribute("passwordexplanation", atktext("password_leaveblankifunchanged"), AF_HIDE_ADD|AF_HIDE_LIST|AF_HIDE_SEARCH|AF_HIDE_VIEW));
    $passwordrestrictions = array(
      "minsize" => atkconfig("password_minsize", 0),
      "minupperchars" => atkconfig("password_minupperchars", 0),
      "minlowerchars" => atkconfig("password_minlowerchars", 0),
      "minalphabeticchars" => atkconfig("password_minalphabeticchars", 0),
      "minnumbers" => atkconfig("password_minnumbers", 0),
      "minspecialchars" => atkconfig("password_minspecialchars", 0)
    );
    $this->add(new atkPasswordAttribute("password", false, AF_HIDE_LIST|AF_PASSWORD_NOVALIDATE, 40, $passwordrestrictions));
  }

  function descriptor_def()
  {
    return "[userid]";
  }

  function getThemes()
  {
    atkimport("atk.utils.atktmpfile");
    $file = new atkTmpFile("themes/available_themes.inc");
    if(!$file->exists())
    {
      $traverser = &atknew('atk.utils.atkdirectorytraverser');
      $themefinder = &atknew('module.employee.utils.themefinder');
      $traverser->addCallbackObject($themefinder);
      $traverser->traverse(atkconfig('atkroot'));
      $themes = $themefinder->getThemes();
      $file->writeAsPhp("themes",$themes);
    }
    else
    {
      include $file->getPath();
    }
    return $themes;
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
