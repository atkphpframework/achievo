<?php

  function pimTodos()
  {
    global $g_user;

    $userid = $g_user["id"];
    $rl = &atknew("atk.recordlist.atkrecordlist");
    $todonode = getNode("todo.todo");
    $todonode->m_flags |= NF_NO_SEARCH;
    $todonode->addFilter("todo.status NOT IN (5,2)");

    $actions["edit"] = "dispatch.php?atknodetype=todo.todo&atkaction=edit&atkselector=[pk]";

    $recs = $todonode->selectDb("assigned_to='".$userid."' AND assigned_to <> 0");
    $todo_assignedtome = atktext("pim_assignedtoyou").":<br>";
    if (count($recs)>0)
    {
      $todo_assignedtome.= $rl->render($todonode,$recs,$actions,false, array('assigned_to','entrydate','projectid'));
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
      $todo_assignedbyme.= $rl->render($todonode,$recs,$actions,false, array('entrydate','projectid'));
    }
    else
    {
      $todo_assignedbyme.= atktext("pim_todo_empty")."<br>";
    }
    $todo_assignedbyme.= '<br>'.href("dispatch.php?atknodetype=todo.todo&atkaction=add",atktext('add'),SESSION_NESTED);

    $res.= '<table border="0">';
    $res.= '<tr>';
    $res.= '<td valign="top">'.$todo_assignedtome.'</td>';
    $res.= '</tr><tr><td>&nbsp;</td></tr><tr>';
    $res.= '<td valign="top">'.$todo_assignedbyme.'</td>';
    $res.= '</tr></table>';
    return $res;
  }

?>