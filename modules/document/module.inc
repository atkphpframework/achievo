<?php

$modifiers[] = "project.project";
userelation("atkonetomanyrelation");

  class mod_document extends achievoModule
  {
    function getNodes()
    {
      registerNode("document.document", array("admin", "add","edit","delete"));
    }

    function project_project_modifier(&$node)
    {
      $node->add(new atkOneToManyRelation("documents","document.document", "project_id", AF_HIDE_LIST|AF_HIDE_SEARCH), "documents");
    }
  }
?>