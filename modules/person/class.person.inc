<?php

/**
 * Module Person
 * Class person
 *
 * This class is a generalisation of employee and contact and is used for schedule_attendees
 * and for the general attribute of employee and contact.
 *
 * $Id$
 * @author L. Willems  <lineke@ibuildings.nl>
 * @version $Revision$
 *
 */

userelation("atkmanytoonerelation");
userelation("atkonetomanyrelation");
useattrib("atkemailattribute");
useattrib("atktextattribute");
useattrib("atklistattribute");
useattrib("atkparserattribute");
useattrib("atknumberattribute");
useattrib("person.useridattribute");

class person extends atkNode
{
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
    $this->m_securityMap["vcard"] = "view";

    $this->setTable("person", "person");
    $this->setOrder("person.role, person.lastname");
    $this->setIndex("lastname");
  }

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

  function descriptor_def()
  {
    return "[lastname], [firstname] ([role])";
  }

  function action_vcard()
  {
    return $this->createVCard($this->m_postvars["id"]);
  }

  function createVCard($id)
  {
    include_once(moduleDir("utils")."vcard.php");
    $v = new vCard();

    $recs = $this->selectDb("id='".$id."'", "", "", "", array("phone", "cellular", "lastname", "firstname",
                                                              "address", "city", "state", "zipcode",
                                                              "country", "email", "remark"));
    $record = $recs[0];

    $v->setPhoneNumber($record["phone"], "PREF;WORK;VOICE");
    $v->setPhoneNumber($record["cellular"], "CELL");
    $v->setName($record["lastname"], $record["firstname"], "", "");
    $v->setAddress("", "", $record["address"], $record["city"], $record["state"], $record["zipcode"], $record["country"]);
    $v->setEmail($record["email"]);
    $v->setNote($record["remark"]);

    $output = $v->getVCard();
    //$filename = $v->getFileName();
    //Remove spaces that would break the vcard filename
    $name = str_replace(" ", "_", $record['firstname']."_".$record['lastname'] );
    $filename = $name.".vcf";

    Header("Content-Disposition: attachment; filename=$filename");
    Header("Content-Length: ".strlen($output));
    Header("Connection: close");
    Header("Content-Type: text/x-vCard; name=$filename");

    echo $output;
  }

}

?>