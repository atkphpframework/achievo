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
 * Scheduler employee view
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage scheduler
 * @since 1.3.0
 */
class scheduler_employeeview extends scheduler_viewday
{
  var $m_employees = array();

  function scheduler_employeeview()
  {
    $this->setView("employee");
  }

  function setEmployees($employees)
  {
    $this->m_employees = $employees;
  }

  function _getNav()
  {
    return "";
  }

  function _dumpMatrix()
  {
    $this->m_viewTitle  = atktext("seluserdate");
    $tmp = $this->_view_header(false);
    $tmp.='<script language="javascript">
    function handleCheck(box, name, on, off)
    {
      el = document.getElementById(name);
      if (box.checked)
      {
        el.value = on;
      }
      else
      {
        el.value = off;
      }
    }

  </script>';
    $tmp.= '<form method="get" action="'.atkSelf().'">'."\n";
    $tmp.= session_form();
    $tmp.= '<input type="hidden" name="view" value="'.$this->m_view.'">';

    $users = $this->_getEmployees($this->m_employees,"checkbox",false);
    $tmp.= '<table>
            <tr><td>
           ';
    $tmp.= $users."<br>";
    $dummy_rec = array("viewdate"=>$this->_getInitialDate());
    $datebox = new atkDateAttribute("viewdate","F d Y","d F Y",0,0,AF_OBLIGATORY);
    $tmp.=$datebox->edit($dummy_rec);
    $tmp.= '&nbsp;<input type="submit" value="'.atktext("refresh").'">'."\n";
    $tmp.= '</td></tr></table></form><hr>';
    if(count($this->m_employees)>0)
    {

      $tmp.= '<span style="align: left">
              <table>';
      $header = '<tr><td bgcolor="#D0D0D0">Employee</td>';
      for($i=0;$i<count($this->m_employees);$i++)
      {
        $header.='<td bgcolor="#D0D0D0">'.$this->m_employeeLookup[$this->m_employees[$i]].'</td>';
      }
      $header.= '</tr>';
      $tmp.=$header;
      $employee_appointments = array();
      for($i=0;$i<count($this->m_employees);$i++)
      {
        $this->getitems($this->m_viewdate,$this->m_viewdate,array($this->m_employees[$i]));
        $employee_appointments[$this->m_employees[$i]]["allday"] = $this->m_allDayItems;
        $employee_appointments[$this->m_employees[$i]]["appointments"] = $this->m_itemMatrix;
      }
      for($i=0;$i<count($this->m_allDayHours);$i++)
      {
        if($this->m_allDayHours[$i]=="12:00") $tmp.=$header;
        $now     = $this->year."-".$this->month."-".$this->day;
        $addurl  = atkSelf()."?atkaction=add&atknodetype=calendar.schedule&date=$now&time=".$this->m_allDayHours[$i];

        $tmp.='<tr>
                <td bgcolor="#D0D0D0">'.href($addurl, "<font size='-1'>".$this->m_allDayHours[$i]."</font>").'</td>';
        for($j=0;$j<count($this->m_employees);$j++)
        {
          if(count($employee_appointments[$this->m_employees[$j]]["allday"])>0)
          {
            $tmp.='<td bgcolor="#FF0000">VOL</td>';
          }
          elseif(count($employee_appointments[$this->m_employees[$j]]["appointments"][$this->m_allDayHours[$i]])>0)
          {
            $tmp.='<td bgcolor="#FF0000">VOL</td>';
          }
          else
          {
            $tmp.='<td bgcolor="#00FF00">Leeg</td>';
          }
        }

        $tmp.='</tr>';
      }
      $tmp.='</table></span>';
    }
    return $tmp;
  }


  function renderView()
  {
    $this->_setUserSettings();
    for($i=0;$i<=23;$i++)
    {
      for($j=0;$j<=55;$j+=$this->m_scheduletime)
      {
        $time = sprintf("%02d:%02d",$i,$j);
        $this->m_allDayHours[] =  $time;
        if($time>=$this->_correct_time($this->m_work_starttime,"down") && $time<$this->_correct_time($this->m_work_endtime,"up"))
          $this->m_workHours[] = $time;
      }
    }
    //$this->getItems('2003-01-01','2003-12-31');
    return $this->_dumpMatrix();
  }

}

?>