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
class email_template extends atkNode
{
  var $m_subject_template = "";
  var $m_plain_template = "";
  var $m_html_template = "";
  var $m_data = array();

  function email_template($name="email_template", $flags=0)
  {
    $this->atkNode($name, $flags|NF_NO_ADD|NF_NO_DELETE);
    $this->add(new atkAttribute("code",AF_HIDE|AF_PRIMARY));
    $this->add(new atkAttribute("module",AF_HIDE));
    $this->add(new atkAttribute("title",AF_READONLY|AF_HIDE_EDIT));
    $this->add(new atkDummyAttribute("help",$this->_getHelpText(),AF_HIDE_LIST));
    $this->add(new atkAttribute("subject",AF_HIDE_LIST));
    $this->add(new atkTextAttribute("plain_body",'',AF_HIDE_LIST));
    $this->add(new atkFckAttribute("html_body",'',AF_HIDE_LIST));
    $this->setTable("email_template");
    $this->setOrder("email_template.title");
  }

  function descriptor_def()
  {
    return "[title]";
  }

  /**
   * Get the help text for the tempalte
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
    return '<b>{$'.$code.'}</b>: '.atktext("help_".$code,"scheduler").'<br/ >';
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
   * Load the the templates for subject, plain_body, html_body
   *
   * @param string $module Module name
   * @param string $code Code of the template
   * @param array $data The data that is needed for the template
   */
  function loadTemplate($module,$code,$data)
  {
    $this->m_data = $data;
    $record = $this->selectDb("email_template.module='".escapeSQL($module)."' AND email_template.code='".escapeSQL($code)."'");
    if(count($record)>0)
    {
      $this->m_subject_template = $record[0]["subject"];
      $this->m_plain_template = $record[0]["plain_body"];
      $this->m_html_template = $record[0]["html_body"];
    }
  }

  /**
   * Get the parsed subject
   *
   * @return string Subject line
   */
  function getSubject()
  {
    return $this->_parseTemplate($this->m_subject_template,$this->m_data);
  }

  /**
   * Get the parsed plain text body
   *
   * @return string Plain text body
   */
  function getPlainBody()
  {
    return $this->_parseTemplate($this->m_plain_template,$this->m_data);
  }

  /**
   * Get the parsed HTML body
   *
   * @return string HTML Body
   */
  function getHtmlBody()
  {
    return $this->_parseTemplate($this->m_html_template,$this->m_data);
  }

  /**
   * Parse the template with the data
   *
   * @access private
   * @param string $template The template
   * @param array $data The data needed for parsing the template
   * @return string Parsed template
   */
  function _parseTemplate($template,$data)
  {
    $tmp_patterns = array_keys($data);
    $patterns = array();
    foreach($tmp_patterns as $pattern)
    {
      $patterns[] = '/{\$'.$pattern.'}/';
    }
    atk_var_dump($patterns,"<h1>PATTERNS</h1>");
    atk_var_dump(array_values($data),"<h1>DATA</h1>");

    return preg_replace($patterns, array_values($data), $template);
  }
}

?>