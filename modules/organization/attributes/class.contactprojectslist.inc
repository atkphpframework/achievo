<?php
  /**
   * contactprojectslist node class file
   *
   * @package achievo
   * @subpackage modules
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2006 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing/ ATK open source license
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * @internal Load used classes
   */
  useattrib("atkdummyattribute");

  /**
   * The node class for contactprojectslist.
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo
   * @subpackage modules
   */
  class contactprojectslist extends atkDummyAttribute
  {

    var $m_status = "active";

    /**
     * contactprojectslist constructor
     */
    function contactprojectslist($name, $status="active", $flags=0)
    {
      $this->atkDummyAttribute($name, "", $flags|AF_DUMMY_SHOW_LABEL);
      $this->m_status = $status;
    }

    /**
     * Returns a html recordlist of projects
     *
     * @param unknown_type $record
     * @param unknown_type $mode
     * @return unknown
     */
    function showProjects($record, $mode)
    {
      $projectnode = &atkGetNode("project.project");
      useattrib("project.filterprojectbycontactattrib");
      $projectnode->add(new filterProjectByContactAttrib($record["id"]));
      $projectnode->m_fuzzyFilters=array("project.status='".$this->m_status."'");
      $recs = $projectnode->selectDb("", "", "", $projectnode->m_listExcludes);
      if (count($recs)>0)
      {
        $rl = &atknew("atk.recordlist.atkrecordlist");
        return $rl->render($projectnode, $recs, $projectnode->defaultActions($mode), RL_NO_SEARCH|RL_NO_SORT);
      }
      return atktext("none");
    }

    /**
     * Edit  record
     * Here it will only return the text, no edit box.
     * @param $record Array with fields
     * @return Parsed string
     */
    function edit($record="", $fieldprefix="")
    {
      return $this->showProjects($record, "relation");
    }

    /**
     * Display a record
     * Here it will only returns the text.
     * @param $record  Array with fields
     * @return Parsed string
     */
    function display($record)
    {
      return $this->showProjects($record, "view");
    }


  }
?>