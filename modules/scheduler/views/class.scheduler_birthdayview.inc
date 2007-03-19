<?php
atkimport("modules.scheduler.views.scheduler_view");

class scheduler_birthdayview extends scheduler_view
{
 /**
  * scheduler yearview constructor
  */
  function scheduler_birthdayview()
  {
    $this->setView("birthday");
  }

 /**
  * navigate through years using prev/next year link
  * @returns html code containing links for previous year, current year and next year
  */
  function _getNav()
  {
    return "";
  }

 /**
  * the actual displaying function
  * @return string Returns a renderd year matrix
  */
  function _dumpMatrix()
  {
    $employees = &atkGetNode("employee.employee");
    $birthdays = $employees->getBirthdates();
    atkimport("modules.utils.dateutil");

    $tmp = $this->_view_header();
    $tmp.='<table cellpadding="5">
           <tr>';
    
    for($i=1;$i<=12;$i++)
    {
      $tmp.='<td valign="top"><b>'.ucfirst(dateutil::monthname($i)).'</b><br><br>';
      if(isset($birthdays[$i]) && is_array($birthdays[$i]))
      {
        foreach($birthdays[$i] as $day => $names)
        {
          foreach($names as $name)
          {
            $tmp.=sprintf("%02s %s",$day,$name)."<br>";
          }
        }
      }
      $tmp.="</td>";
      if($i%3==0) $tmp.="</tr><tr>";
    }
    $tmp.="</tr></table>";

    return $tmp;
  }
 /**
  * Render the view
  * @return string A renderd view
  */
  function renderView()
  {
    $this->_setUserSettings();
    return $this->_dumpMatrix();
  }

}




?>