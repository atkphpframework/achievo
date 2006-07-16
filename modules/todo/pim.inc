<?php

  function pimTodos()
  {
    global $g_user;

    $userid = $g_user["id"];
    $rl = &atknew("atk.recordlist.atkrecordlist");

    $todonode = &atkGetNode("todo.todo");
    $todonode->m_flags |= NF_NO_SEARCH;
    $todonode->m_action="admin";
    $todonode->addFilter("todo.status NOT IN (5,2)");

    $actions = array();
    $actions["edit"] = "dispatch.php?atknodetype=todo.todo&atkaction=edit&atkselector=[pk]";
    $actions["complete"] = "dispatch.php?atknodetype=todo.todo&atkaction=completed&atkselector=[pk]";

    $recs = $todonode->selectDb("todo.assigned_to='$userid' AND todo.assigned_to <> 0  AND ((todo.private=0) OR (todo.private is null) OR (todo.private=1 and todo.owner='$userid'))");
    $todo_assignedtome = atktext("pim_assignedtoyou").":<br>";
    if (count($recs)>0)
    {
      $todo_assignedtome.= $rl->render($todonode,$recs,$actions,RL_MRA|RL_NO_EXTENDED_SEARCH|RL_NO_SEARCH|RL_NO_SORT, array('assigned_to','entrydate'),"todoassignedtoyou",array(),"todoassignedtoyou");
    }
    else
    {
      $todo_assignedtome.= atktext("pim_todo_empty")."<br>";
    }
    $todo_assignedtome.= '<br>'.href("dispatch.php?atknodetype=todo.todo&atkaction=add&atkfilter=".rawurlencode("assigned_to.id='$userid'"),atktext('add'),SESSION_NESTED);

    $todo_assignedbyme = atktext("pim_assignedbyyou").":<br>";
    $recs = $todonode->selectDb("todo.owner='$userid' AND assigned_to<>'$userid'");
    if (count($recs)>0)
    {
      $todo_assignedbyme.= $rl->render($todonode,$recs,$actions,RL_MRA|RL_NO_EXTENDED_SEARCH|RL_NO_SEARCH|RL_NO_SORT, array('entrydate'),"todoassignedbyyou",array(),"todoassignedbyyou");
    }
    else
    {
      $todo_assignedbyme.= atktext("pim_todo_empty")."<br>";
    }
    $todo_assignedbyme.= '<br>'.href("dispatch.php?atknodetype=todo.todo&atkaction=add",atktext('add'),SESSION_NESTED);

    $res = '<table border="0">';
    $res.= '<tr>';
    $res.= '<td valign="top">'.$todo_assignedtome.'</td>';
    $res.= '</tr><tr><td>&nbsp;</td></tr><tr>';
    $res.= '<td valign="top">'.$todo_assignedbyme.'</td>';
    $res.= '</tr></table>';
    return $res;
  }

?>