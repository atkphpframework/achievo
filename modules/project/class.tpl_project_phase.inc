<?php

userelation("atkmanytoonerelation");

class tpl_project_phase extends atkNode
{
  function tpl_project_phase()
  {
    $this->atkNode("tpl_project_phase",NF_NO_EDIT); // node() constructor is *not* called automatically!

    $this->addAllowedAction(array("add","delete","edit","view"));

    $this->add(new atkManyToOneRelation("projectid","project.tpl_project",AF_PRIMARY|AF_HIDE_LIST));
    $this->add(new atkManyToOneRelation("phaseid","project.tpl_phase",AF_PRIMARY));

    $this->setTable("tpl_project_phase");
  }

  function descriptor_def()
  {
    return "[phaseid.name]";
  }

  function postDel($rec)
  {
    // delete dependencies..
    $db = &atkGetDb();
    $delquery = "DELETE FROM
                 tpl_dependency
               WHERE
                 projectid = ".$rec["projectid"]["id"]."
                 AND (phaseid_row = ".$rec["phaseid"]["id"]." OR phaseid_col = ".$rec["phaseid"]["id"].")";

    $db->query($delquery);
    return true;
  }
}
?>