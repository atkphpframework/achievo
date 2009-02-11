<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage employee
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */

useattrib("atkdummyattribute");
useattrib("atknumberattribute");
useattrib("atkpasswordattribute");
useattrib("atklistattribute");
useattrib('employee.adminlangselectattribute');
/**
 * Class for managing user preferences
 * @package achievo
 * @subpackage employee
 */
class userprefs extends atkNode
{
  /**
   * Constructor
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
    $this->add(new adminLangSelectAttribute("lng", AF_APPLY));

    $this->setTable("person", "person");
    $this->setDescriptorTemplate('[userid]');
  }

  /**
   * Initial values
   *
   * @return array
   */
  function initial_values()
  {
    return array("lng"=>atkconfig("language"));
  }

  /**
   * Add password attribute to the node
   *
   */
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
  
  
  /**
   * Check rights for user settings, user can only edit his own settings
   * 
   * @param string $action The action we want to allow
   * @param array $record The record for the action
   * @return boolean Are we allowed to execute the action
   */
  function allowed($action,$record="")
  {
    global $ATK_VARS;
    $userid = atkGetUser('id');
    $selector = decodeKeyValueSet($ATK_VARS['atkselector']);
    if(array_key_exists('person.id',$selector) && $selector['person.id']==$userid) return true;
    return false;  
  }
  

  /**
   * Find available themes
   *
   * @return array
   * @todo use atkCache for this
   */
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

  /**
   * Refresh screen when theme is changes
   *
   * @param array $record Record
   * @return boolean
   */
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