<?php

  // class for selecting an employee from employees that you supervise.

  userelation("atkmanytoonerelation");

  class myEmployeesAttribute extends atkManyToOneRelation
  {
    function myEmployeesAttribute($name, $any_user_privilege="", $flags=0)
    {
      $this->atkManyToOneRelation($name, "employee.employee", $flags);

      $any_user = false;
      if ($any_user_privilege!="")
      {
        list($module, $node, $action) = explode(".", $any_user_privilege);

        $sm = &atkGetSecurityManager();
        $any_user = $sm->allowed($module.".".$node, $action);
      }

      $filter = "status='active'";

      if (!$any_user)
      {
        // set a filter
        $user = getUser();
        $filter.= " AND supervisor=".$user["id"];
      }
      $this->setDestinationFilter($filter);
    }
  }

?>