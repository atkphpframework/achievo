<?php

class tpl_project_phase extends atkNode
{
  function tpl_project_phase()
  {
    $this->atkNode("tpl_project_phase",NF_NO_EDIT); // node() constructor is *not* called automatically!
      
    $this->addAllowedAction(array("add","delete","edit","view"));

    $this->addAttribute(new atkManyToOneRelation("projectid","tpl_project",AF_PRIMARY|AF_HIDE_LIST));
    $this->addAttribute(new atkManyToOneRelation("phaseid","tpl_phase",AF_PRIMARY)); 

    $this->setTable("tpl_project_phase","tpl_project_phase");
    atkdebug("tpl_project_phase::tpl_project_phase()");
  }
    
  function descriptor_def()
  {
    return "[phaseid.name]";
  }
    
  function postDel($rec)
  {
    // delete dependencies..      
    global $g_db;
    $delquery = "DELETE FROM 
                 tpl_dependency 
               WHERE 
                 projectid = ".$rec["projectid"]["id"]."
                 AND (phaseid_row = ".$rec["phaseid"]["id"]." OR phaseid_col = ".$rec["phaseid"]["id"].")";
    atkdebug($delquery);
    $g_db->query($delquery);
  }    
}
?>