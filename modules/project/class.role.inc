<?php
  /**
   * Role class file
   *
   * This class is used to define roles for persons.
   * At this moment it is not possible to delete roles that
   * are referenced in one or more project_person records
   *
   * @package achievo
   * @subpackage modules
   *
   * @author L. Willems  <lineke@ibuildings.nl>
   *
   * @copyright (c) 2006 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision$
   * $Id$
   */
  useattrib("atknumberattribute");
  /**
   * The node class for Role.
   *
   * @author L. Willems  <lineke@ibuildings.nl>
   * @package achievo
   * @subpackage modules
   */
  class role extends atkNode
  {

    /**
     * Constructor
     */
    function role()
    {
      $this->atkNode("role");

      $this->add(new atkNumberAttribute("id", AF_AUTOKEY));
      $this->add(new atkAttribute("name",AF_FORCE_LOAD|AF_OBLIGATORY));
      $this->add(new atkAttribute("description",AF_FORCE_LOAD|AF_OBLIGATORY));

      $this->setTable("role");
      $this->setOrder("role.name ASC");
      $this->setDescriptorTemplate("[name]");
    }

    /**
     * Determines the set of actions that can be performed on a record
     *
     * @param Array $record Record containing role data for which the actions should be determined
     * @param Array $actions Array containing the default set of records which can be modified as it is passed by reference
     */
    function recordActions($record, &$actions)
    {
      $db = &atkGetDb();
      $role_id = $record["id"];
      // Check if role is being used.
      $recs = $db->getrows("select count(1) as cnt from project_person where role='".$role_id."'");
      if ($recs[0]["cnt"]>0)
      {
        unset($actions["delete"]);
      }
    }
  }
?>