<?php
  /**
   * Module Resource planning
   * Class employee_project
   *
   * Here the employees can be linked to a project. The employees can only be planned on projects they are linked to
   *
   * $Id$
   *
   * @author Ludo M. Beumer  <ludo@ibuildings.nl>
   * @version $Revision$
   *
   */

  userelation("atkmanytoonerelation");

  class employee_project extends atkNode
  {
    function employee_project()
    {
      $this->atkNode("employee_project", NF_NO_EDIT);

      $this->add(new atkManyToOneRelation("employeeid","employee.employee",AF_PRIMARY|AF_RELATION_AUTOLINK));
      $this->add(new atkManyToOneRelation("projectid","project.project",AF_PRIMARY));
      $this->setTable("employee_project");
    }
  }

?>