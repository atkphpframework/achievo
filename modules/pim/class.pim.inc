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

      $g_layout->initGUI();
      $g_layout->ui_top(text("title_pim"));
      $res = '<br>';

      $pimItems = atkHarvestModules("getPimItems");

      foreach ($pimItems as $name => $contents)
      {
        $res.= '<b>'.text('pim_'.$name).'</b><br><br>';
        $res.= $contents;
        $res.= '<br><br>';
      }

      $res.='<br>';

      $g_layout->output($res);

      $g_layout->ui_bottom();

      $g_layout->page("PIM");
    }
  }

?>