<?php

/**
 * Module Project
 * Class document
 *
 * This class makes it possible to add documents to a project and upload this document to the webserver.
 *
 * $Id$
 * @author L. Willems  <lineke@ibuildings.nl>
 * @version $Revision$
 *
 */

  useattrib ("atkFileAttribute");
  useattrib ("atkDateAttribute");
  useattrib ("atklistattribute");
  userelation ("atkmanytoonerelation");

  class document extends atkNode
  {
    function document()
    {
      $this->atkNode("document");

      $this->add(new atkAttribute("id", AF_AUTOKEY));
      $this->add(new atkAttribute("name", AF_OBLIGATORY));
      $this->add(new atkFileAttribute("filename",atkconfig("documentpath"),AF_FILE_POPUP|AF_FILE_NO_SELECT|AF_OBLIGATORY|AF_FILE_PHYSICAL_DELETE));
      $this->add(new atkAttribute("owner",AF_READONLY|AF_HIDE_ADD|AF_HIDE_EDIT));
      $this->add(new atkManyToOneRelation("project_id", "project.project", AF_HIDE));
      $this->add(new atkDateAttribute("entrydate",AF_READONLY|AF_HIDE_ADD|AF_HIDE_EDIT));
      $this->add(new atkAttribute("version", AF_OBLIGATORY));
      $this->add(new atkListAttribute("status", array(text("concept"), text("internal"), text("accepted"))));

      $this->setTable("document","document");
      $this->setOrder("name");
    }

    function initial_values()
    {
      global $g_user;
      return array("owner"=>$g_user["name"],
                 "entrydate"=>array("year"=>date("Y"),
                                    "month"=>date("m"),
                                    "day"=>date("d")));
    }

    function status_display($rec)
    {
      return text($rec["status"]);
    }

  }


?>
