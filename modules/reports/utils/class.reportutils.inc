<?php
/**
 * One line description of the class
 * 
 * Full description of the class
 *
 * @author Dennis-Jan Broerse <dennisjan@ibuildings.nl>
 * @package package
 * @subpackage subpackage
 *
 */
class reportutils
{
  
  /**
   * Constructor
   *
   */
  function reportutils()
  {
    
  }  
  
  function get_employees($user_id, $allemployees = false)
  {
    $user = getUser();

    if (is_array($user))
    {
      if ($allemployees)
      $records = $this->getEmployeesArray();
      else
      $records = $this->getEmployeesArray($user['id']);

      $employee_code = $this->getEmployeeCode($records, $user_id, $user);

      return $employee_code;
    }
  }

  function getEmployeesArray($managerid = '')
  {
    $records = $this->getEmployeesFromDb($managerid);

    if (is_array($records))
    {
      foreach ($records as $employee)
      {
        if (!is_null($employee['id']))
        $subset = $this->getEmployeesArray($employee['id']);
      }

      if (!empty($subset))
      return array_merge($records, $subset);
      else
      return $records;
    }
  }


  function getEmployeesFromDb($managerid = '')
  {
    if ($managerid != '')
    $where = " AND person.supervisor='$managerid'";

    $db = &atkGetDb();

    $sql = "SELECT person.id,person.lastname,person.userid, person.firstname
              FROM person
              WHERE person.status='active' AND person.role='employee'$where 
              ORDER BY person.lastname
             ";

    $records = $db->getrows($sql);

    if (is_array($records) && count($records) > 0)
    return $records;

    return null;
  }

  function getEmployeeCode($records, $user_id, $user)
  {
    if ($user_id == "new" || !$user_id || $user_id=="")
    $sel = " SELECTED";
    else
    $sel = "";

    $employee_code='<OPTION VALUE="all"'.$sel.'>'.atktext("allusers");
    $employee_code .= '<OPTION VALUE="'.addslashes($user["id"]).'" '.$sel.'>'.$user["lastname"].', '.$user["firstname"].'</OPTION>';

    for($i=0,$_i=count($records);$i<$_i;$i++)
    {
      if($user_id==$records[$i]["id"])
      $sel="SELECTED";
      else
      $sel="";

      $employee_code.='<OPTION VALUE="'.addslashes($records[$i]["id"]).'" '.$sel.'>'.$records[$i]["lastname"].', '.$records[$i]["firstname"].'</OPTION>';
    }

    return $employee_code;
  }

  function getLevelEmployees($userid)
  {
    atkdebug("getLevelEmployees function called.");
    $user = getUser();

    if (is_array($user))
    {
      $harvest = atkHarvestModules("getEmpArray", '', true);

      if (is_array($harvest) && count($harvest) > 0)
      {
        if (moduleExists('advancedsecurity'))
        $empArray = $harvest['advancedsecurity'];
        
        $empCode = $this->getEmployeeCode($empArray,$userid, $user);
        
        return $empCode;
      }
      else 
      {
        atkwarning("Harvester getEmpArray is empty. Returning only himself.");
        
        $empCode = $this->getEmployeeCode(array(),$userid, $user);
        
        return $empCode;
      }
    }
  }
  
  function get_functionlevels($selected)
  {
    $harvest = atkHarvestModules("getFuncLevelDropDown", '', true);

    if (is_array($harvest) && count($harvest) > 0)
    {
      if (moduleExists('advancedsecurity'))
      $func_code = $harvest['advancedsecurity'];
    }
    else
    {
      $db = &atkGetDb();
      $sql = "  SELECT DISTINCT functionlevel.id as id, functionlevel.name as name
                FROM functionlevel
                LEFT JOIN person ON (person.functionlevel = functionlevel.id)";
      $records = $db->getrows($sql);

      $sel = !$selected || $selected == "all" ? " selected" : "";
      $func_code = '<SELECT name="functionlevelswitch"><OPTION VALUE="all"'.$sel.'>'.atktext("allfunctionlevels").'</OPTION>';
      foreach($records as $record)
      {
        $sel = $selected == $record['id'] ? " selected" : "";
        $func_code .= '<OPTION VALUE="'.$record['id'].'"'.$sel.'>'.$record['name'].'</OPTION>';
      }
      $func_code .= '</SELECT>';
    }
    return $func_code;
  }
}
?>