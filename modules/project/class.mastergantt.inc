<?php
  /**
   * projectstaus  node class file
   *
   * @package achievo
   * @subpackage project
   *
   * @author ivo <ivo@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * @internal includes
   */
  useattrib("atkattribute");
  useattrib("atkdateattribute");

  atkimport("module.utils.dateutil");

  include_once("achievotools.inc");

  /**
   * The node class for project status
   *
   * @author ivo <ivo@ibuildings.nl>
   * @package achievo
   * @subpackage reports
   */
  class mastergantt extends atkNode
  {
    /**
     * projectstatus constructor
     */
    function mastergantt()
    {
      $this->atkNode("projectstatus");
      $this->setSecurityAlias("project.project");      
    }

    function getFilterRecord()
    {
      static $s_record = NULL;
      global $g_sessionManager;

      if ($s_record==NULL)
      {
        $s_record = array();
        $s_record["coordinator"] = $g_sessionManager->pageVar("coordinator");
        $s_record["from"] = $g_sessionManager->pageVar("from");
        $s_record["to"] = $g_sessionManager->pageVar("to");
        $s_record["plannedbooked"] = $g_sessionManager->pageVar("plannedbooked");

        // convert attribute html values to internal values
        $attrs = &$this->getFilterAttrs();
        foreach (array_keys($attrs) as $attribname)
        {
          $p_attrib = &$attrs[$attribname];
          $s_record[$attribname] = &$p_attrib->fetchValue($s_record);
        }


        $go = $g_sessionManager->pageVar("go");
        if ($go!=1)
        {
          // initial loading. We'll put the from/to fields to reasonable defaults.

          $s_record["from"] = dateUtil::str2arr(dateUtil::startOfWeek(date("Ymd")));

          // next month
          $next_month = mktime(12,0,0,date("m")+1, date("d"), date("Y"));
          $s_record["to"] = dateUtil::str2arr(dateUtil::endOfWeek(date("Ymd", $next_month)));
        }
      }
      return $s_record;
    }

    function &getFilterAttrs()
    {
      useattrib("employee.myemployeesattribute");

      $attrs["coordinator"] = &new myEmployeesAttribute("coordinator", "project.project.any_project");
      
      useattrib("atklistattribute");
      $attrs["plannedbooked"] = &new atkListAttribute("plannedbooked", array("planned", "booked"), "", AF_OBLIGATORY|AF_LIST_NO_NULL_ITEM);
      
      $attrs["from"] = &new atkDateAttribute("from");
      $attrs["to"] = &new atkDateAttribute("to");

      return $attrs;
    }

    function getFilterBar()
    {
      $record = $this->getFilterRecord();

      $attrs = &$this->getFilterAttrs();

      $output = '<form action="dispatch.php" method="get">';
      $output.= session_form();
      $output.= '<input type="hidden" name="go" value="1">'; //trigger to prevent loading the first time

      if ($this->hasAnyProjectPrivilege())
      {
        $output.= atktext("coordinator").": ".$attrs["coordinator"]->edit($record)." ";
      }
      
      $output.= atktext("display").": ".$attrs["plannedbooked"]->edit($record)." ";

      $output.= "<br/>";

      $output.= atktext("showonlyphasesfrom", "reports")." ".$attrs["from"]->edit($record)." ";
      $output.= atktext("to")." ".$attrs["to"]->edit($record);

      $output.= ' <input type="submit" value="'.atktext("refresh").'">';
      $output.= '</form>';

      return $output;
    }
    
    function hasAnyProjectPrivilege()
    {
      return $this->allowed("any_project");
    }

    function action_planning()
    {           
      $filter = $this->getFilterRecord();
      
      $content = $this->getFilterBar();
      
      if ($this->m_postvars["go"]==1)
      {
        if (!$this->hasAnyProjectPrivilege())
        {
          $user = getUser();
          $coordinator = $user["id"];
        }
        else 
        {
          $coordinator = $filter["coordinator"]["id"];
        }
        $imgurl = moduleDir("project")."mastergantt.php?coordinator=".$coordinator.
                  "&from=".dateUtil::arr2str($filter["from"])."&to=".dateUtil::arr2str($filter["to"]).
                  "&plannedbooked=".$filter["plannedbooked"];
              
        $content.= '<br/><br/><img src="'.$imgurl.'" border="0"/><br/><br/>';
      }

      $ui = &$this->getUi();
      $box = $ui->renderBox(array("title"=>"Master Gantt", "content"=>$content));

      $this->addStyle("style.css");
      $page = &$this->getPage();
      $page->addContent($box);
    }



  }
?>