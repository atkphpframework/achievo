<?php

  function pimTodos()
  {
    global $g_user, $g_layout, $g_db;

    $me = $g_user["name"];
    $sql = "SELECT id FROM person WHERE userid = '".$me."' AND role='employee'";
    $userid = $g_db->getRows($sql);

    $todonode = getNode("todo.todo");
    $todonode->m_flags |= NF_NO_SEARCH;
    $todonode->addFilter("todo.status NOT IN (5,2)");

    $actions["edit"] = "dispatch.php?atknodetype=todo.todo&atkaction=edit&atkselector=[pk]";

    if ($me != "administrator") $recs = $todonode->selectDb("assigned_to=".$userid[0]["id"]." AND assigned_to <> 0"); else $recs = $todonode->selectDb("assigned_to='".$me."' AND assigned_to <> 0");
    $todo_assignedtome = text("pim_assignedtoyou").":<br>";
    if (count($recs)>0)
    {
      $todo_assignedtome.= $todonode->recordList($recs,$actions,false, array('assigned_to','entrydate','projectid'));
    }
    else
    {
      $todo_assignedtome.= text("pim_todo_empty")."<br>";
    }
    $todo_assignedtome.= '<br>'.href("dispatch.php?atknodetype=todo.todo&atkaction=add&atkfilter=".rawurlencode("assigned_to.userid='$me'"),text('add'),SESSION_NESTED);

    $todo_assignedbyme = text("pim_assignedbyyou").":<br>";
    $recs = $todonode->selectDb("todo.owner='$me' AND assigned_to<>'$me'");
    if (count($recs)>0)
    {
      $todo_assignedbyme.= $todonode->recordList($recs,$actions,false, array('entrydate','projectid'));
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