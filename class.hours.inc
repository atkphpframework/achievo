<?php

include_once "class.projectphaseattrib.inc";
include_once "class.projectattrib.inc";

function time_format($time)
{
  if ($time==0) return "&nbsp;";
  return sprintf("%02d",floor($time/60)).':'.sprintf("%02d",$time%60);
}    
  
function date_format($datestamp)
{
  return date("m-d",$datestamp)." (".text(strtolower(date("D",$datestamp))).")";
}

class hours extends atkNode
{
  function hours()
  {
    global $g_user,$atkaction,$atkorderby, $g_sessionManager, $weekview, $viewdate; 
    
    // weekview and viewdate are variables that we need to remember..    
    $weekview = $g_sessionManager->stackVar("weekview");
    $viewdate = $g_sessionManager->stackVar("viewdate");
    
    // hoursurvey has extra sticky vars..
    if ($atkaction=="hoursurvey")
    {
      // TODO: this doesn't work any more with the new session management code.
      // $this->addStickyVar(Array("projectid","userid","phaseid","activityid"));    
    }
    
    $this->atkNode("hours"); // node() constructor is *not* called automatically!
    
    $this->addAttribute(new atkDateAttribute("date"));
    if($atkaction!='hoursurvey')
    {
      $this->addFilter("userid",strtolower($g_user["name"])); 
    }
    if($atkaction!='hoursurvey'&&$atkaction!='edit')
    {
      if ($weekview!=1)
      {
        if ($viewdate=="") $viewdate = strftime("%Y-%m-%d");
        $this->addFilter("date",$viewdate); 
      }        
    }

    $this->addAttribute(new atkAttribute("id",AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
                
    if($atkaction=="hoursurvey"||$weekview==1)
    {
      $this->addAttribute(new atkDateAttribute("date","F d Y","d F Y", 0, date("Ymd")));
    }
    else
    {
      $this->addAttribute(new atkDateAttribute("date","F d Y","d F Y", 0, date("Ymd"), AF_HIDE_LIST));
    }
    
    if ($atkaction=="hoursurvey")
    {
      $this->addAttribute(new atkAttribute("userid"));
    }
    else
    {
      $this->addAttribute(new atkAttribute("userid",AF_HIDE));
    }
    
    $this->addAttribute(new atkProjectPhaseAttribute("phaseid",AF_OBLIGATORY));
    
    $this->addAttribute(new atkListAttribute("activityid",Array("dummy","dummy","dummy","dummy","dummy"),Array(),AF_OBLIGATORY));      
    
    $this->addAttribute(new atkAttribute("remark"));      
    
    $this->addAttribute(new atkTimeAttribute("time",'15m', '10h',AF_OBLIGATORY|AF_TOTAL));
                
    $this->addAttribute(new atkDateAttribute("entrydate",AF_HIDE));      
                
    $this->addAttribute(new atkDummyAttribute("javascript", "seebelow", AF_NO_LABEL|AF_HIDE_LIST));

    if($atkaction=="hoursurvey")
    {
      $this->setOrder("id");
    }
    else
    {
      if($atkorderby!="") $this->setOrder($atkorderby);
    }
    
    $this->setTable("hours","hours");
   
  }
      
  function initial_values()
  {
     global $g_user, $viewdate;

     $initial_date = Array();    
     
     if ($viewdate!="")
     {
       $initial_date = Array("year"=>substr($viewdate,0,4),
                             "month"=>substr($viewdate,5,2),
                             "day"=>substr($viewdate,8,2));
     }
     else
     {
       $initial_date = Array("year"=>date("Y"),
                             "month"=>date("m"),
                             "day"=>date("d"));
     }
     
     return array("userid"=>strtolower($g_user["name"]),
                  "entrydate"=>Array("year"=>date("Y"),
                             "month"=>date("m"),
                             "day"=>date("d")),
                  "date"=>$initial_date);
  }
  
  function activityid_display($rec)
  {
    // activityname is loaded in the phaseid join, so we get the info from there..
    return $rec["phaseid"]["activityname"];
  }
  
     
  function action_hoursurvey()
  {      
    include 'hoursurvey.inc';
  }
  
  function javascript_edit($rec)
  {
    global $currentphase, $currentactivity, $weekview, $viewdate;
    
    if (!isset($currentphase)||$currentphase=="") $currentphase=0;
    if (!isset($currentactivity)||$currentactivity=="") $currentactivity =0;
    $result = '<script language="javascript"> set_activities('.$currentphase.','.$currentactivity.'); </script>';
        
    return $result;
  }
   
  // override the admin action, because we have the weekview/dayview switch..
  function action_admin()
  {      
    global $weekview;
    $this->addPage();
    if ($weekview!=1)
    {
      $this->adminPage();
    }
    else
    {
      $this->weekview();
    }      
  }
  
  function adminHeader()
  {
    global $weekview, $viewdate;
    if ($weekview!=1)
    {
      $viewdatestamp = mktime(0,0,0,substr($viewdate,5,2),substr($viewdate,8,2),substr($viewdate,0,4));
      $yesterday = date('Y-m-d',$viewdatestamp-86400);
      $tomorrow = date('Y-m-d',$viewdatestamp+86400);
    
      $res = href('dispatch.php?viewdate='.$yesterday, text("houradmin_previousday"));
      $res.='&nbsp;&nbsp;<b>'.date_format($viewdatestamp).'</b>';
      if ($viewdate<date('Y-m-d')) // If current is not today we can also move forward
        $res.='&nbsp;&nbsp;'.href('dispatch.php?viewdate='.$tomorrow, text("houradmin_nextday"));
      $res.= '<br><br>'.href('dispatch.php?viewdate='.$viewdate.'&weekview=1', text("houradmin_gotoweekview"));
    }
    return $res;
  }
  
  function weekview()
  {
    global $g_layout, $viewdate, $config_overtimethreshold;
    
    if ($viewdate!="")
    {
      $viewtime = mktime(0,0,0,substr($viewdate,5,2),substr($viewdate,8,2),substr($viewdate,0,4));
    }
    else
    {
      $viewdate = date("Y-m-d");
      $viewtime = time();
    }
    
    $g_layout->ui_top(text('title_houradmin_weekview').' '.strftime('%V',$viewtime));

    $weekday = strftime("%w",$viewtime);
    if ($weekday==0) $weekday=7;

    for ($i=0; $i<=8;$i++)
    {
      $weekdates[$i]['date']=date("Y-m-d" ,$viewtime-(86400*($weekday-$i)));
      $weekdates[$i]['day']=text(strtolower(date("D" ,$viewtime-(86400*($weekday-$i)))));
      $weekdates[$i]['stamp']=$viewtime-(86400*($weekday-$i));
    }
    
    $week = Array();
    $projtotals = Array();
    $daytotals = Array();

    $data = $this->selectDb("date >= '".$weekdates[1]['date']."' AND date <= '".$weekdates[7]['date']."'","","",$this->m_listExcludes);
    
    for ($i=0;$i<count($data);$i++)
    {
      $rec = $data[$i];
      $key = $rec["phaseid"]["projname"]."-".$rec["phaseid"]["phasename"];
      $date = $rec["date"]["year"]."-".$rec["date"]["month"]."-".$rec["date"]["day"]; 
      $time = ($rec["time"]["hours"]*60)+$rec["time"]["minutes"];
      
      $week[$key][$date] += $time;      
      $projtotals[$key] += $time;
      $daytotals[$date] += $time;
      $total += $time;
    }
    
    // First a line with previous and next week links..
    $prevnextlinks = '<br>'.href('dispatch.php?weekview=1&viewdate='.$weekdates[0]['date'], text('houradmin_previousweek'));
    if ($weekdates[8]['date'] <= date("Y-m-d"))
    {
      $prevnextlinks.='&nbsp;&nbsp;'.href('dispatch.php?weekview=1&viewdate='.$weekdates[8]['date'],text('houradmin_nextweek'));
    }
    
    $g_layout->output($prevnextlinks);
    
    $g_layout->output($g_layout->data_top());
    
    $g_layout->output('<tr>');
    
    $g_layout->td_datatitle(text('project').'/'.text('phase'));
    for ($i=1;$i<=7;$i++)
    {        
      if ($weekdates[$i]['date'] <= date("Y-m-d"))
      {
        $url = 'dispatch.php?viewdate='.$weekdates[$i]['date']."&weekview=0";
        $g_layout->td_datatitle(href($url,text($weekdates[$i]['day'])).'<br>('.substr($weekdates[$i]['date'],5,2).'-'.substr($weekdates[$i]['date'],8,2).')','align="center"');
      }
      else
      {
        $g_layout->td_datatitle(text($weekdates[$i]['day']).'<br>('.substr($weekdates[$i]['date'],5,2).'-'.substr($weekdates[$i]['date'],8,2).')','align="center"');
      }
      
    }
    $g_layout->td_datatitle(text('total'));
    
    $g_layout->output('</tr>');
    
    while (list($proj, $times)=each($week))
    {      
      $g_layout->output('<tr>');
      $g_layout->td($proj);
      for ($i=1;$i<=7;$i++)
      {
        $g_layout->td(time_format($times[$weekdates[$i]['date']]), 'align="center"');
      }
      $g_layout->td(time_format($projtotals[$proj]), 'align="center"');
      $g_layout->output('</tr>');
    }
    
    $g_layout->output('<tr>');
    
    $g_layout->td_datatitle(text('total'));
    for ($i=1;$i<=7;$i++)
    {        
      if ($daytotals[$weekdates[$i]['date']]>$config_overtimethreshold)
      {
        $color = "#FF0000";
      }
      else
      {
        $color = "#009900";
      }
      $g_layout->td_datatitle('<font color="'.$color.'">'.time_format($daytotals[$weekdates[$i]['date']]).'</font>','align="center"');
      
    }
    $g_layout->td_datatitle(time_format($total), 'align="center"');
    
    $g_layout->output('</tr>');
    
    $g_layout->output($g_layout->data_bottom());
    
    $g_layout->ui_bottom();
  } 
  
  function postAdd($rec)
  {
    // if we add a record on a date that is not the currently viewed date, we jump
    // to that date.
    // TODO this doesn't work anymore due to the fact that we move down in the session stack
    // where viewdate still has it's old value (do we need a way to alter the previous session?)
    $this->m_postvars["viewdate"]=$rec["date"]["year"]."-".sprintf("%02d",$rec["date"]["month"])."-".sprintf("%02d",$rec["date"]["day"]);
  }
  
 function postUpdate($rec)
  {
    // if we edit a record and set it to a date that is not the currently viewed date, we jump
    // to that date.
    // TODO this doesn't work anymore due to the fact that we move down in the session stack
    // where viewdate still has it's old value (do we need a way to alter the previous session?)
    $this->m_postvars["viewdate"]=$rec["date"]["year"]."-".sprintf("%02d",$rec["date"]["month"])."-".sprintf("%02d",$rec["date"]["day"]);
  }
      
}
?>