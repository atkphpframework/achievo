<?php

include_once "class.dependencyattrib.inc";

// When a new project is saved, you can base it on a template, i.e. select a set of 
// default activities. To accomplish this, we make a new attribute based on a ManyToOneRelation
// that isn't stored in the db, but used to add records to the projectactivities table.
class projecttemplateAttribute extends atkManyToOneRelation
{
  function projecttemplateAttribute($name, $target, $flags)
  {
    $this->atkManyToOneRelation($name, $target, $flags);
  }

  function addToQuery($query)
  {
    // do nothing
  }
  
  function store($db, $record, $type)
  {
    // Todo, here we must save the activities from the template to the projectactivities table
    if ($type == "add")
    {

      // phase templates
      $query = "SELECT tpl_phase.id, tpl_phase.name, tpl_phase.description FROM tpl_phase,tpl_project_phase
                WHERE tpl_phase.id = tpl_project_phase.phaseid AND tpl_project_phase.projectid = ".$record['template']['id'];
      atkdebug($query);
      $phases = $db->getRows($query);
              
      // loop to insert phases , create reference table
      for ($i=0;$i<count($phases);$i++)
      {
         $new_id = $db->nextid('phase');
         $id_table[$phases[$i]['id']] = $new_id;
      
         $query = "INSERT INTO phase (id, name, projectid, description) 
                   VALUES ('".$new_id."','".$phases[$i]['name']."','".$record['id']."','".$phases[$i]['description']."')";
         atkdebug($query);
         $db->query($query);
         
         //activities per phase 
         $query = "INSERT INTO phase_activity (activityid, phaseid) 
                   SELECT activityid, ".$new_id." 
                   FROM tpl_phase_activity 
                   WHERE phaseid = ".$phases[$i]['id'];
         atkdebug($query);
         $db->query($query);
      }
         
      // dependency templates
      $query = "SELECT phaseid_row, phaseid_col FROM tpl_dependency
                WHERE tpl_dependency.projectid = ".$record['template']['id'];
      $dependencies = $db->getRows($query);
       
      // loop to insert dependencies, uses reference table
      for ($i=0;$i<count($dependencies);$i++)
      {
         $row = $id_table[$dependencies[$i]['phaseid_row']];   
         $col = $id_table[$dependencies[$i]['phaseid_col']];      
      
         if ($row!='' && $col!='')
         {
            $query = "INSERT INTO dependency (phaseid_row, phaseid_col) 
                      VALUES ('".$row."','".$col."')";
            $db->query($query);
         }
      }
    }
  }
}

class project extends atkNode
{
  function project()
  {
    $this->atkNode("project",NF_EDITAFTERADD);

    $this->addAttribute(new atkAttribute("id",AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkAttribute("name",AF_UNIQUE|AF_OBLIGATORY|AF_SEARCHABLE));
    $this->addAttribute(new atkManyToOneRelation("customer","customer",AF_HIDE_SELECT|AF_HIDE_ADD));
    $this->addAttribute(new atkManyToOneRelation("coordinator","employee",AF_SEARCHABLE|AF_HIDE_ADD));
    $this->addAttribute(new atkTextAttribute("description",AF_HIDE_LIST|AF_HIDE_ADD));
    $this->addAttribute(new atkDateAttribute("startdate","F d Y","d F Y",0,0,AF_HIDE_ADD));    
    
    // If we are in project administration mode, we show all projects. Otherwise, we only
    // show active projects.      
    if ($GLOBALS["atknodetype"]=="project"&& in_array($GLOBALS["atkaction"],array("admin","add","update","edit")))
    {          
      $this->addAttribute(new atkListAttribute("status",array("active","nonactive","archived"),array(),AF_HIDE_ADD|AF_SEARCHABLE));
    }
    else
    {
      $this->addAttribute(new atkListAttribute("status",array("active","nonactive","archived"),array(),AF_HIDE));
      $this->addFilter("project.status","active");
    }

    $this->addAttribute(new atkOneToManyRelation("phase","phase","projectid", AF_HIDE_LIST));
    $this->addAttribute(new dependencyAttribute("dependencies",AF_HIDE_ADD|AF_HIDE_LIST|AF_BLANKLABEL));
    $this->addAttribute(new projecttemplateAttribute("template","tpl_project", AF_HIDE_LIST|AF_HIDE_EDIT));

    $this->setTable("project","project");
    $this->setOrder("name");

    atkdebug("project::project()");
  }

  function descriptor_def()
  {
    return "[name]";
  }
  
  function initial_values()
  {
    return array("startdate"=>array("year"=>date(Y),"month"=>date(m),"day"=>date(d)),"status"=>"active");
  }

  function action_stats()
  {
    include_once 'projectstats.inc';   // MILESTONES FUNCTIONS     
  }
  
  function action_planning()
  {
    include_once 'projectplanning.inc';   // MILESTONES FUNCTIONS     
  }
  
  function action_billsetup()
  {
    include_once 'billsetup.inc';
  } 
  
  function action_billing()
  {
    include_once 'projectbilling.inc';
  } 
  
  function postDel($rec)
  {
    // TODO: delete phases and dependencies, and hours (???)
  }
}
?>