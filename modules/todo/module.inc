<?php

  class mod_todo extends achievoModule
  {
    function getNodes()
    {
      registerNode("todo.todo", array("admin", "add", "edit", "delete"));
    }

    function getMenuItems()
    {
      $selectedproject = $_REQUEST["selectedproject"];
      if ($selectedproject == 0)
      {
        $this->menuitem("project_todoview", dispatch_url("todo.project_todoview", "admin"), "projectmanagement", array("todo.todo", "admin"));
      }
      else
      {
        $this->menuitem("project_todoview", dispatch_url("todo.project_todoview", "edit", array("atkselector" => "project.id=".$selectedproject)), "projectmanagement", array("todo.todo", "admin"));
      }
    }
    
    function getDependencies($mode)
    {
      return array("project");
    }

    function getPimItems()
    {
      include_once(moduleDir("todo")."pim.inc");
      return array("todos"=>pimTodos());
    }
  }

?>