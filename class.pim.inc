<?php   

  class pim extends atkNode
  {
    function pim()
    {
      $this->atkNode("pim",NF_NO_SECURITY);
    }
    
    function action_pim()
    {
      global $g_user, $g_db, $g_layout;
  
      $me = $g_user["name"];         
      
      $g_layout->ui_top(text("title_pim"));
      $res = '<br>';
      
      // SCHEDULER
      
      $res.= '<b>'.text('pim_schedule').':</b><br><br>';
      
      $schedulenode = getNode("schedule");
      $schedulenode->m_flags |= NF_NO_SEARCH;
      $recs = $schedulenode->selectDb("owner='$me' AND now()>=startdate AND now()<=enddate");                  
      
      $actions = array();
      
      if (count($recs)>0)
      {
        $todayschedule = $schedulenode->recordList($recs,$actions,false, array('publicitem','allday','location','scheduletype','enddate'));
      }
      else
      {
        $todayschedule = text('pim_schedule_empty').'<br>';
      }
      $todayschedule.='<br>'.href("dispatch.php?atknodetype=schedule&atkaction=admin",text('pim_gotoschedule'),SESSION_NESTED);         
  
      $res.= $g_layout->ret_table_simple();
      $res.= '<tr>';
      $res.= $g_layout->ret_td('&nbsp;','width="15"'); // spacer      
      $res.= $g_layout->ret_td($todayschedule, 'valign="top"');    
      $res.= '</tr></table>';
      
      // TODO's
  
      $res.= '<br><br><b>'.text('pim_todos').':</b><br><br>';
      
      $todonode = getNode("todo");
      $todonode->m_flags |= NF_NO_SEARCH;
      $todonode->addFilter("todo.status NOT IN (5,2)");
      
      $actions["edit"] = "dispatch.php?atknodetype=todo&atkaction=edit&atkselector=[pk]";
      
      $recs = $todonode->selectDb("assigned_to='$me'");
      $todo_assignedtome = text("pim_assignedtoyou").":<br>";          
      if (count($recs)>0)
      {        
        $todo_assignedtome.= $todonode->recordList($recs,$actions,false, array('assigned_to','entrydate','projectid'));
      }      
      else
      {
        $todo_assignedtome.= text("pim_todo_empty")."<br>";
      }
      $todo_assignedtome.= '<br>'.href("dispatch.php?atknodetype=todo&atkaction=add&atkfilter=".rawurlencode("assigned_to.userid='$me'"),text('add'),SESSION_NESTED);
      
      $todo_assignedbyme = text("pim_assignedbyyou").":<br>";          
      $recs = $todonode->selectDb("owner='$me' AND assigned_to<>'$me'");
      if (count($recs)>0)
      {        
        $todo_assignedbyme.= $todonode->recordList($recs,$actions,false, array('entrydate','projectid'));
      }
      else
      {
        $todo_assignedbyme.= text("pim_todo_empty")."<br>";
      }
      $todo_assignedbyme.= '<br>'.href("dispatch.php?atknodetype=todo&atkaction=add",text('add'),SESSION_NESTED);         
      
      $res.= $g_layout->ret_table_simple();
      $res.= '<tr>';
      $res.= $g_layout->ret_td('&nbsp;','width="15"'); // spacer
      $res.= $g_layout->ret_td($todo_assignedtome, 'valign="top"');
      $res.= $g_layout->ret_td('&nbsp;','width="15"'); // spacer
      $res.= $g_layout->ret_td($todo_assignedbyme, 'valign="top"');    
      $res.= '</tr></table>';
      
      
      // TIME REGISTRATION
      $res.= '<br><br><b>'.text('pim_timeregistration').':</b><br><br>';
      
      $hoursnode = getNode("hours");                        
      
      $actions = array();
      
      $recs = $hoursnode->selectDb();
      
      if (count($recs)>0)
      {        
        $todayhours.= $hoursnode->recordList($recs,$actions,false, array());
      }      
      else
      {
        $todayhours.= text('pim_timeregistration_empty').'<br>';
      }
      
      $todayhours.= '<br>'.href("dispatch.php?atknodetype=hours&atkaction=add",text('add'),SESSION_NESTED);
      $todayhours.= ' &nbsp; '.href("dispatch.php?atknodetype=hours&atkaction=admin",text('pim_gototimeregistration'),SESSION_NESTED);
                                  
      $res.= $g_layout->ret_table_simple();
      $res.= '<tr>';
      $res.= $g_layout->ret_td('&nbsp;','width="15"'); // spacer
      $res.= $g_layout->ret_td($todayhours, 'valign="top"');      
      $res.= '</tr></table>';   
      
      // OUTPUT EVERYTHING
      
      $res.='<br>';
  
      $g_layout->output($res);
            
  
      $g_layout->ui_bottom();
  
      $g_layout->page("PIM");
    }
  }
  
?>