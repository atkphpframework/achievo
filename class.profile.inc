<?php
/**
 * NEED EXTRA WORK!!! 
 */

// for editing profiles we create a custom attribute
class profileAttribute extends atkAttribute
{
  function profileAttribute($name, $flags)
  {
    $this->atkAttribute($name, $flags);
  }
  
  function load($db, $record)
  {
    $query = "SELECT * FROM accessright WHERE entity='".$record["id"]."'";
    atkdebug($query);
    $result = Array();
    $rows = $db->getrows($query);
    for ($i=0;$i<count($rows);$i++)
    {     
      $result[$rows[$i]["node"]][] = $rows[$i]["action"];
    }
    return $result;
  }
  
  function store($db, $record, $mode)
  {
    $delquery = "DELETE FROM accessright WHERE entity='".$record["id"]."'";
    atkdebug($delquery);
    $db->query($delquery);
    
    $checked = $record[$this->fieldName()];
    while (list($node, $actions)=each($checked))
    {
      for ($i=0;$i<count($actions);$i++)
      {
        $query = "INSERT INTO accessright (node, action, entity) ";
        $query.= "VALUES ('".$node."','".$actions[$i]."','".$record["id"]."')";
        atkdebug($query);
        $db->query($query);
      }
    }
  }
  
  function edit($record)
  {
    /* get nodes */
    require_once("atk/atknodetools.inc");
    include_once("config.nodes.inc");

    /* get permissions */
    $checked = $record[$this->fieldName()];
    
    /* what nodes? global app and modules! */
    $nodes = $GLOBALS["g_nodes"];
    foreach ($GLOBALS["g_modules"] as $module)
    {
      $mod_inst = new $module();
      foreach ($mod_inst->getNodes() as $node => $actionarr) $nodes["$module.$node"] = $actionarr;
    }
    
    /* build edit screen */
    while (list($node, $actionarr) = each($nodes))
    {
      $checked_actions = $checked[$node];
      if (is_array($actionarr))
      {
        $result.="<b>".text(getNodeType($node))."</b><br>";
        for ($i=0;$i<count($actionarr);$i++)
        {
          $action = $actionarr[$i];
          if (substr($action,0,1)=="*") $action=substr($action,1);
          $result.='<input type="checkbox" name="'.$this->formName().'[]" value="'.$node.".".$action.'" ';
          if (is_array($checked_actions)&&in_array($action, $checked_actions)) $result.=" checked";
          $result.='> '.text($action).'&nbsp;&nbsp;&nbsp;';
        }
        $result.="<br><br>";
      }
    }
    return $result;
  }
  
  function fetchValue($postvars)
  {
    $checkboxes = $postvars[$this->fieldName()];
    $actions = Array();
    for($i=0;$i<count($checkboxes);$i++)
    {
      $data = explode(".", $checkboxes[$i]);
      if (is_array($data) && count($data) == 2) $actions[$data[0]][] = $data[1];
      else if (is_array($data) && count($data) == 3) $actions[$data[0].".".$data[1]][] = $data[2];
    }
    return $actions;
  }
}


class profile extends atkNode
{
  function profile()
  {
    $this->atkNode("profile", NF_EDITAFTERADD);

    $this->addAttribute(new atkAttribute("id",AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkAttribute("name",AF_OBLIGATORY|AF_UNIQUE|AF_SEARCHABLE));      
    $this->addAttribute(new atkDummyAttribute("profile_explanation",text("profile_explanation"),AF_HIDE_LIST|AF_HIDE_ADD));
    $this->addAttribute(new profileAttribute("accessrights",AF_HIDE_LIST|AF_HIDE_ADD|AF_BLANKLABEL));

    $this->setTable("profile","profile");
    $this->setOrder("name");
  }

  function descriptor_def()
  {
    return "[name]";
  }
}
?>