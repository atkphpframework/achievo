<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage scheduler
 *
 * @copyright (c)2008 Sandy Pleyte
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */

atkimport("modules.scheduler.views.scheduler_view");

/**
 * Scheduler birthday view
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage scheduler
 * @since 1.3.0
 */
class scheduler_birthdayview extends scheduler_view
{
   var $m_view = "birthday";

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

    $tmp.="<br>";
    $startmonth = date("m");
    for($i=0;$i<12;$i++)
    {
      $month=$startmonth+$i;
      if($month>12) $month-=12;
      if(isset($birthdays[$month]) && is_array($birthdays[$month]))
      {
        $tmp.='<b>'.ucfirst(atktext(dateutil::monthname($month),'atk')).'</b> ('.count($birthdays[$month]).')<br>';
        foreach($birthdays[$month] as $day => $users)
        {
          foreach($users as $info)
          {
            $age=0;
            if(atkconfig::get('scheduler','birthday_show_age',false))
            {
              $tmp.=sprintf("%02d-%02d: %s (%d)",$day,$month,$info["name"],$this->calculateAge($info["birthday"]))."<br>";
            }
            else
            {
              $tmp.=sprintf("%02d-%02d: %s",$day,$month,$info["name"])."<br>";
            }
          }
        }
        $tmp.="<br>";

      }
    }

    return $tmp;
  }

  function calculateAge($birthdate)
  {
     if(!isset($birthdate["year"]) && $birthdate["year"]=="") return "-";
     $current_year = date("Y");
     if($birthdate["month"]<date("m")) // || ($birthdate["month"]==date("m") &&$birthdate["day"]<date("d")))
       $current_year+=1;

     return ($current_year-$birthdate["year"]);
  }

  /**
   * Birthday view doesn't need a selector
   */
  function _getSelector()
  {
    return '';
  }


 /**
  * Render the view
  * @return string A renderd view
  */
  function _render()
  {
    return $this->_dumpMatrix();
  }

}




?>