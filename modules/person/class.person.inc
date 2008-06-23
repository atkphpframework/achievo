<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage person
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */


userelation("atkmanytoonerelation");
userelation("atkonetomanyrelation");
useattrib("atkemailattribute");
useattrib("atktextattribute");
useattrib("atklistattribute");
useattrib("atkparserattribute");
useattrib("atknumberattribute");
useattrib("person.useridattribute");

/**
 * This class is a generalisation of employee and contact and is used for schedule_attendees
 * and for the general attribute of employee and contact.
 *
 * @author L. Willems  <lineke@ibuildings.nl>
 * @package achievo
 * @subpackage person
 */
class person extends atkNode
{
  /**
   * constructor
   *
   * @param string $name Name
   * @param int $flags Flags
   */
  function person($name="person", $flags=0)
  {
    $this->atkNode($name, $flags|NF_ADD_LINK);

    $this->add(new atkNumberAttribute("id" ,AF_AUTOKEY));
    $this->add(new atkManyToOneRelation("title_id", "person.title", AF_RELATION_AUTOLINK|AF_HIDE_LIST));
    $this->add(new useridAttribute("userid" ,AF_UNIQUE|AF_SEARCHABLE), 1); // first field
    $this->add(new atkAttribute("lastname",AF_SEARCHABLE|AF_OBLIGATORY, 50));
    $this->add(new atkAttribute("firstname",AF_HIDE_LIST|AF_FORCE_LOAD, 50));
    $this->add(new atkAttribute("initials", AF_HIDE_LIST|AF_FORCE_LOAD, 50));
    $this->add(new atkAttribute("address",AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("zipcode",AF_HIDE_LIST, 20));
    $this->add(new atkAttribute("city",AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("state",AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("country",AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("phone", 0, 20));
    $this->add(new atkAttribute("cellular", AF_HIDE_LIST|AF_FORCE_LOAD, 20));
    $this->add(new atkAttribute("fax", AF_HIDE_LIST|AF_FORCE_LOAD, 20));
    $this->add(new atkEmailAttribute("email", false, 0, 50));
    $this->add(new atkAttribute("function", AF_HIDE_LIST, 50));
    $this->add(new atkTextAttribute("remark", TEXT_LARGE, AF_HIDE_LIST));
    $this->add(new atkAttribute("role", AF_OBLIGATORY|AF_HIDE, 15)); //discriminator
    $this->add(new atkParserAttribute("vcard", href("?atkaction=vcard&id=[id]", atktext("vcard"), SESSION_NESTED), AF_NO_LABEL|AF_HIDE_ADD), NULL, 10000);

    $this->addSecurityMap("vcard", "view");
    $this->setTable("person", "person");
    $this->setOrder("person.role, person.lastname");
    $this->setIndex("lastname");
    $this->setDescriptorTemplate("[title_id.title] [lastname], [firstname] ([role])");
  }

  /**
   * Override lastname display, in list mode show first name if available
   *
   * @param array $record Record
   * @param string $mode Mode
   * @return string
   */
  function lastname_display($record, $mode)
  {
    if ($mode=="list")
    {
      return $record["lastname"].($record["firstname"]!=""?", ".$record["firstname"]:"");
    }
    else
    {
      return $this->m_attribList["lastname"]->display($record, $mode);
    }
  }

  /**
   * Phone display override, show all phone numers in list mode
   *
   * @param array $record Record
   * @param string $mode Mode
   * @return string
   */
  function phone_display($record, $mode)
  {
    if ($mode=="list") // in listmode, group the numbers
    {
      $res = array();
      if ($record["phone"]!="")
      {
        $res[] = "<b>".substr(atktext("phone"),0,1).":</b> ".$record["phone"];
      }
      if ($record["cellular"]!="")
      {
        $res[] = "<b>".substr(atktext("cellular"),0,1).": </b>".$record["cellular"];
      }
      if ($record["fax"]!="")
      {
        $res[] = "<b>".substr(atktext("fax"),0,1).": </b>".$record["fax"];
      }
      return implode("<BR>", $res);
    }
    else
    {
      return $this->m_attribList["phone"]->display($record, $mode);
    }
  }

  /**
   * Vcard action
   *
   * @param object $handler Handler
   */
  function action_vcard(&$handler)
  {
    // Get the record
    $id = atkArrayNvl($this->m_postvars, "id");
    $persons = $this->selectDb("person.id='".$id."'", "", "", "", "", "view");

    // Create the vcard
    include_once(moduleDir("utils")."vcard.php");
    $vcard = new vCard();
    $this->assignVcard($vcard, $persons[0]);

    // Output the vcard and stop further script execution
    $vcard->outputVCard();
    exit;
  }

  /**
   * Assign values to vcard
   *
   * @param object $vcard Vcard object
   * @param array $record Record
   */
  function assignVcard(&$vcard, $record)
  {
    $vcard->setPhoneNumber($record["phone"], "HOME;VOICE");
    $vcard->setPhoneNumber($record["cellular"], "HOME;CELL");
    $vcard->setPhoneNumber($record["fax"], "HOME;FAX");
    $vcard->setName($record["lastname"], $record["firstname"], "", $record["title_id"]["title"]);
    $vcard->setFormattedName(trim($record["title_id"]["title"] . " " . $record["firstname"] . " " . $record["lastname"]));
    $vcard->setAddress("", "", $record["address"], $record["city"], $record["state"], $record["zipcode"], $record["country"], "HOME");
    $vcard->setEmail($record["email"]);
    $vcard->setNote($record["remark"]);
    $vcard->setProperty("TITLE", $record["function"]);
  }
}

?>