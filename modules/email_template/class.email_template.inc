<?php
/**
 * Email template class
 *
 * @package achievo
 * @subpackage modules
 *
 * @author sandy <sandy@achievo.org>
 *
 * @version $Revision$
 * $Id$
 */

useattrib("atktextattribute");
useattrib("atkfckattribute");
useattrib("atkdummyattribute");
useattrib("atkupdatestampattribute");
useattrib("atkupdatedbyattribute");

class email_template extends atkNode
{

  /**
   * Module name
   *
   * @var string
   */
  var $m_module = "";

  /**
   * Template code
   *
   * @var string
   */
  var $m_template_code="";

  /**
   * User id for getting name and email addres
   *
   * @var string
   */
  var $m_user_id="";

  /**
   * From email address
   *
   * @var string
   */
  var $m_from_email = "";

  /**
   * From name
   *
   * @var string
   */
  var $m_from_name = "";
  
  var $m_customReceiver = "";

  /**
   * Array with all the data we need for the template
   *
   * @var array
   */
  var $m_data = array();

  /**
   * Constructor
   *
   * @param string $name Node name
   * @param int $flags Flags
   */
  function email_template($name="email_template", $flags=0)
  {
    $this->atkNode($name, $flags|NF_NO_ADD|NF_NO_DELETE);
    $this->add(new atkAttribute("code",AF_HIDE|AF_PRIMARY));
    $this->add(new atkAttribute("module",AF_HIDE));
    $this->add(new atkAttribute("title",AF_READONLY|AF_HIDE_EDIT));
    $this->add(new atkDummyAttribute("help",$this->_getHelpText(),AF_HIDE_LIST));
    $this->add(new atkAttribute("subject",AF_HIDE_LIST));
    $this->add(new atkTextAttribute("plain_body",'',AF_HIDE_LIST));
    $this->add(new atkFckAttribute("html_body",'',AF_HIDE_LIST,array('ToolbarSet'=>'Basic')));
    $this->add(new atkUpdateStampAttribute("lastupdatedon",AF_HIDE_LIST),"history");
    $this->add(new atkUpdatedByAttribute("lastupdatedby",AF_HIDE_LIST),"history");
    $this->setTable("email_template");
    $this->setOrder("email_template.title");
  }

  /**
   * Save module name in a var for using it with the translations
   *
   * @param object $handler Handler object
   * @param array $record Current record
   * @param bool $locked Locked or not
   * @return string Edit page
   */
  function editPage(&$handler,$record,$locked)
  {
    $this->m_module = $record["module"];
    return $handler->editPage($record,$locked);
  }

  function descriptor_def()
  {
    return "[title]";
  }

  function setModule($module)
  {
    $this->m_module = $module;
  }

  function setFromEmail($email)
  {
    $this->m_from_email = $email;
  }

  function setFromName($name)
  {
    $this->m_from_name = $name;
  }

  /**
   * Get the help text for the template
   *
   * @access private
   * @return string Help text
   */
  function _getHelpText()
  {
    $codes = $this->getValidCodes();
    if(count($codes)==0) return '';

    $ret="";
    $ret.=atkText("You can use certain variables which get substituted with the data of the event.")."<br /><br />";
    $ret.=atkText("Substitutions and their meanings:")."<br />";
    foreach($codes as $code)
    {
      $ret.=$this->_getHelpLine($code);
    }
    $ret.="<br />";
    return $ret;
  }

  /**
   * Get a help line for the code
   *
   * @access private
   * @param string $code
   * @return string A help line for the help text
   */
  function _getHelpLine($code)
  {
    return '<b>{$'.$code.'}</b>: '.atktext("help_".$code,$this->m_module).' - '.$this->m_module.'<br/ >';
  }

  /**
   * Get Valid codes that can be used in the template
   *
   * @return array Array with the valid codes
   */
  function getValidCodes()
  {
    return array();
  }

  /**
   * Load data
   *
   */
  function loadData()
  {
  }

  /**
   * Get the parsed subject
   *
   * @return string Subject line
   */
  function _getSubject()
  {
    $smarty = &atkSmarty::getInstance();
    $smarty->assign($this->m_data);
    $compile_id = $this->m_template_code."_".$this->m_module;
    return $smarty->fetch("templatedb:".$this->m_template_code."/".$this->m_module."/subject",'',$compile_id);
  }

  /**
   * Get the parsed plain text body
   *
   * @return string Plain text body
   */
  function _getPlainBody()
  {
    $smarty = &atkSmarty::getInstance();
    $smarty->assign($this->m_data);
    $compile_id = $this->m_template_code."_".$this->m_module;
    return $smarty->fetch("templatedb:".$this->m_template_code."/".$this->m_module."/plain_body",'',$compile_id);
  }

  /**
   * Get the parsed HTML body
   *
   * @return string HTML Body
   */
  function _getHtmlBody()
  {
    $smarty = &atkSmarty::getInstance();
    $smarty->assign($this->m_data);
    $compile_id = $this->m_template_code."_".$this->m_module;
    return $smarty->fetch("templatedb:".$this->m_template_code."/".$this->m_module."/html_body",'',$compile_id);
  }



  /**
   * Set some initial values
   *
   * @param string $template_code The code of the template
   * @param int $userid The user id for getting the email address
   * @param string $module The module name to override the default one
   */
  function initialize($template_code,$userid,$module="")
  {
    $this->m_template_code=$template_code;
    $this->m_user_id = $userid;
    if($module!="") $this->m_module=$module;
  }

  /**
   * Get user name and email
   *
   * @return mixed An array with the username and email or else false
   */
  function _getUser()
  {
    if($this->m_user_id!="")
    {
      $sql = "SELECT email, firstname, lastname FROM person WHERE id='".$this->m_user_id."'";
      $db = &atkGetDb();
      $nrow = $db->getrows($sql);
      if(count($nrow)==1)
      {
        return array("email"=>$nrow[0]["email"],
                    "name"=>$nrow[0]["firstname"]." ".$nrow[0]["lastname"]);
      }
    }
    return false;
  }
  
  function setCustomerReceiver($email,$username)
  {
    $this->m_customReceiver = array("email"=>$email,"name"=>$username);
  }

  /**
   * Send the mail
   *
   * @return boolean The result of the send mail
   */
  function send()
  {

    $user = $this->_getUser();
    if(is_array($user) && $user["email"])
    {
      $mail = atknew("atk.utils.atkMailer");

      $mail->From     = $this->m_from_email;
      $mail->FromName = $this->m_from_name;
      if(is_array($this->m_customReceiver))
      {
        $mail->AddAddress($this->m_customReceiver["email"],$this->m_customReceiver["name"]);
      }
      else 
      {
        $mail->AddAddress($user["email"], $user["name"]);
      }
      $mail->Subject  = $this->_getSubject();
      $mail->isHTML(true);
      $mail->Body     = $this->_getHtmlBody();
      $mail->AltBody  = $this->_getPlainBody();
      atkdebug("SEND MAIL:<hr>".$mail->Body."<hr>");
      $ret = $mail->Send();
      return $ret;
    }
    else
    {
      atkdebug(atktext("no_user_found"));
    }
    return false;

  }
}

?>