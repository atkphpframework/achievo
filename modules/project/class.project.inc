<?php

  include_once("achievotools.inc");  
  userelation("atkmanytoonerelation");
  userelation("atkonetomanyrelation");
  useattrib("atktextattribute");
  useattrib("atkdateattribute");
  useattrib("atklistattribute");
  useattrib("project.dependencyattrib");
  useattrib("project.projecttemplateattribute");  

  class project extends atkNode
  {
    function project()
    {
      global $ATK_VARS, $g_sessionManager;

      $this->atkNode("project",NF_EDITAFTERADD);

      $this->add(new atkAttribute("id",AF_AUTOKEY));
      $this->add(new atkAttribute("name",AF_UNIQUE|AF_OBLIGATORY|AF_SEARCHABLE));
      $this->add(new atkAttribute("abbreviation", AF_SEARCHABLE|AF_HIDE_LIST));
      $this->add(new atkManyToOneRelation("customer","organization.customer",AF_HIDE_SELECT|AF_HIDE_ADD|AF_RELATION_AUTOLINK));

      $preFilter = new atkManyToOneRelation("contact","organization.contact",AF_HIDE_ADD|AF_HIDE_LIST|AF_RELATION_AUTOLINK);
      $preFilter->m_destinationFilter = "company='[customer.id]'";
      $this->add($preFilter);

      $this->add(new atkManyToOneRelation("coordinator","employee.employee",AF_SEARCHABLE|AF_HIDE_ADD|AF_RELATION_AUTOLINK));
      $this->add(new atkTextAttribute("description",AF_HIDE_LIST|AF_HIDE_ADD));
      $this->add(new atkAttribute("fixed_price",AF_HIDE_LIST|AF_HIDE_ADD));
      $this->add(new atkDateAttribute("startdate","F d Y","d F Y",0,0,AF_HIDE_ADD));
      $this->add(new atkDateAttribute("enddate","F d Y","d F Y",0,0,AF_HIDE_ADD|AF_HIDE_LIST));

      // If we are in project administration mode, we show all projects. Otherwise, we only
      // show active projects.
      if ($ATK_VARS["atknodetype"]=="project"&& in_array($ATK_VARS["atkaction"],array("admin","add","update","edit","search")))
      {
        $this->add(new atkListAttribute("status",array("active","nonactive","archived"),array(),AF_HIDE_ADD|AF_SEARCHABLE));
      }
      else
      {
        $this->add(new atkListAttribute("status",array("active","nonactive","archived"),array(),AF_HIDE));
        $this->addFilter("project.status","active");
      }

      $this->add(new atkOneToManyRelation("phase","project.phase","projectid",AF_HIDE_LIST|AF_CASCADE_DELETE));
      // $this->addAttribute(new atkOneToManyRelation("Todo","todo","projectid", AF_HIDE_LIST|AF_CASCADE_DELETE));
      // $this->addAttribute(new atkOneToManyRelation("project_notes","project_notes","projectid", AF_HIDE|AF_CASCADE_DELETE));

      $this->add(new dependencyAttribute("dependencies",AF_HIDE_ADD|AF_HIDE_LIST|AF_BLANKLABEL));
      $this->add(new projecttemplateAttribute("template","project.tpl_project", AF_HIDE_LIST|AF_HIDE_EDIT|AF_HIDE_SEARCH));
      
      $this->add(new atkOneToManyRelation("employees","project.employee_project", "projectid", AF_HIDE_LIST));

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