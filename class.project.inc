<?php

  include_once "class.dependencyattrib.inc";
  include_once("achievotools.inc");

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
      if ($record['template']['id']!="")
      {
        // phase templates
        $query = "SELECT tpl_phase.id, tpl_phase.name, tpl_phase.description FROM tpl_phase,tpl_project_phase
                  WHERE tpl_phase.id = tpl_project_phase.phaseid AND tpl_project_phase.projectid = ".$record['template']['id'];

        $phases = $db->getRows($query);

        // loop to insert phases , create reference table
        for ($i=0;$i<count($phases);$i++)
        {
           $new_id = $db->nextid('phase');
           $id_table[$phases[$i]['id']] = $new_id;

           $query = "INSERT INTO phase (id, name, status, projectid, description)
                     VALUES ('".$new_id."','".escapeSQL($phases[$i]['name'])."','active','".$record['id']."','".escapeSQL($phases[$i]['description'])."')";

           $db->query($query);

           //activities per phase
           $query = "INSERT INTO phase_activity (activityid, phaseid)
                     SELECT activityid, ".$new_id."
                     FROM tpl_phase_activity
                     WHERE phaseid = ".$phases[$i]['id'];

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
}

class project extends atkNode
{
  function project()
  {
    global $ATK_VARS, $g_sessionManager;

    $this->atkNode("project",NF_EDITAFTERADD);
    $preFilter = new atkManyToOneRelation("contact","contact",AF_HIDE_ADD|AF_HIDE_LIST);
    $preFilter->m_destinationFilter = "company='[customer.id]'";

    $this->addAttribute(new atkAttribute("id",AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkAttribute("name",AF_UNIQUE|AF_OBLIGATORY|AF_SEARCHABLE));
    $this->addAttribute(new atkAttribute("abbreviation", AF_UNIQUE|AF__OBLIGATORY|AF_SEARCHABLE|AF_HIDE_LIST));
    $this->addAttribute(new atkManyToOneRelation("customer","customer",AF_HIDE_SELECT|AF_HIDE_ADD));
    $this->addAttribute($preFilter);
    $this->addAttribute(new atkManyToOneRelation("coordinator","employee",AF_SEARCHABLE|AF_HIDE_ADD));
    $this->addAttribute(new atkTextAttribute("description",AF_HIDE_LIST|AF_HIDE_ADD));
    $this->addAttribute(new atkAttribute("fixed_price",AF_HIDE_LIST|AF_HIDE_ADD));
    $this->addAttribute(new atkDateAttribute("startdate","F d Y","d F Y",0,0,AF_HIDE_ADD));
    $this->addAttribute(new atkDateAttribute("enddate","F d Y","d F Y",0,0,AF_HIDE_ADD|AF_HIDE_LIST));

    // If we are in project administration mode, we show all projects. Otherwise, we only
    // show active projects.
    if ($ATK_VARS["atknodetype"]=="project"&& in_array($ATK_VARS["atkaction"],array("admin","add","update","edit","search")))
    {
      $this->addAttribute(new atkListAttribute("status",array("active","nonactive","archived"),array(),AF_HIDE_ADD|AF_SEARCHABLE));
    }
    else
    {
      $this->addAttribute(new atkListAttribute("status",array("active","nonactive","archived"),array(),AF_HIDE));
      $this->addFilter("project.status","active");
    }

    $this->addAttribute(new atkOneToManyRelation("phase","phase","projectid",AF_HIDE_LIST|AF_CASCADE_DELETE));
    // $this->addAttribute(new atkOneToManyRelation("Todo","todo","projectid", AF_HIDE_LIST|AF_CASCADE_DELETE));
    // $this->addAttribute(new atkOneToManyRelation("project_notes","project_notes","projectid", AF_HIDE|AF_CASCADE_DELETE));

    $this->addAttribute(new dependencyAttribute("dependencies",AF_HIDE_ADD|AF_HIDE_LIST|AF_BLANKLABEL));
    $this->addAttribute(new projecttemplateAttribute("template","tpl_project", AF_HIDE_LIST|AF_HIDE_EDIT|AF_HIDE_SEARCH));

    $this->setTable("project","project");
    $this->setOrder("name");
    $this->setIndex("name");

//    updateSelectedProjects();

    atkdebug("project::project()");
  }

  function descriptor_def()
  {
    return "[name]";
  }

  function initial_values()
  {
    return array("startdate"=>array("year"=>date(Y),
                                    "month"=>date(m),
                                    "day"=>date(d)),
                 "enddate"=>array("year"=>(date(Y)+1),
                                  "month"=>date(m),
                                  "day"=>date(d)),
                 "status"=>"active");
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
    // Phases, dependancies, todo's, notes, activities are permenantly deleted
    // trough the AF_CASCADE_DELETE flag
    // At the moment a project is also permanently deleted. This wil corrupt de hours survey.
    // Therefore NF_DELETED_FLAG should be realised for hours, projects and
    // employees.

  }

  function fixed_price_edit($rec)
  {
    return $this->m_attribList["fixed_price"]->edit($rec)." ".atkconfig("currency_symbol");
  }


}
?>
