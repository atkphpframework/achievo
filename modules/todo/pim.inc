<?php

  function pimTodos()
  {
    global $g_user, $g_layout, $g_db;

    $userid = $g_user["id"];
    $rl = &atknew("atk.recordlist.atkrecordlist");
    $todonode = getNode("todo.todo");
    $todonode->m_flags |= NF_NO_SEARCH;
    $todonode->addFilter("todo.status NOT IN (5,2)");

    $actions["edit"] = "dispatch.php?atknodetype=todo.todo&atkaction=edit&atkselector=[pk]";

    $recs = $todonode->selectDb("assigned_to='".$userid."' AND assigned_to <> 0"); 
    $todo_assignedtome = text("pim_assignedtoyou").":<br>";
    if (count($recs)>0)
    {
      $todo_assignedtome.= $rl->render($todonode,$recs,$actions,false, array('assigned_to','entrydate','projectid'));
    }
    else
    {
      $todo_assignedtome.= text("pim_todo_empty")."<br>";
    }
    $todo_assignedtome.= '<br>'.href("dispatch.php?atknodetype=todo.todo&atkaction=add&atkfilter=".rawurlencode("assigned_to.userid='$userid'"),text('add'),SESSION_NESTED);

    $todo_assignedbyme = text("pim_assignedbyyou").":<br>";
    $recs = $todonode->selectDb("todo.owner='$userid' AND assigned_to<>'$userid'");
    if (count($recs)>0)
    {
      $todo_assignedbyme.= $rl->render($todonode,$recs,$actions,false, array('entrydate','projectid'));
    }
    else
    {
      $todo_assignedbyme.= text("pim_todo_empty")."<br>";
    }
    $todo_assignedbyme.= '<br>'.href("dispatch.php?atknodetype=todo.todo&atkaction=add",text('add'),SESSION_NESTED);

    $res.= $g_layout->ret_table_simple();
    $res.= '<tr>';
    $res.= $g_layout->ret_td('&nbsp;','width="15"'); // spacer
    $res.= $g_layout->ret_td($todo_assignedtome, 'valign="top"');
    $res.= $g_layout->ret_td('&nbsp;','width="15"'); // spacer
    $res.= $g_layout->ret_td($todo_assignedbyme, 'valign="top"');
    $res.= '</tr></table>';
    return $res;
  }

?>
