<?php

  class pim extends atkNode
  {
    function pim()
    {
      $this->atkNode("pim",NF_NO_SECURITY);
    }

    function action_pim(&$handler)
    {
      $pimItems = atkHarvestModules("getPimItems","",true);
      $userpimitems = &atknew("module.pim.userpimitems");
      $user = getuser();
      $userDbItems = $userpimitems->selectDb("userpimitems.userid='".$user["id"]."'","","",array("id","userid"));
      $res = array();
      if(count($userDbItems)==0)
      {
        $res[atktext("title_pim","pim")] = '<br>'.atkText("pim_nopimitems","pim").'<br><br>';
      }
      else
      {
        $userItems = array();
        foreach($userDbItems as $key => $item)
        {
            $userItems[] = $item["pimitem"];
        }
        $res = "";
        foreach($pimItems as $modname => $item)
        {
          foreach($item as $itemName => $itemContent)
          {
            atkdebug("Check: ".$modname.'_'.$itemName);
            if(in_array($modname.'_'.$itemName,$userItems))
            {
              $module = &getModule($modname);
              if (is_object($module))
              {
                if (method_exists($module, $itemContent))
                {
                  $itemContent = $module->$itemContent();
                }
                $res[atktext(array("pim_$itemName", $itemName), $modname)] = $itemContent;
              }
            }
          }
        }
      }
      $page = &$this->getPage();
      $page->addContent($this->genericPage(atktext("title_pim"), $res));
    }

    function getAdminPimContent()
    {
      // Generate the content
      $employeeadminlink = href(dispatch_url("employee.employee", "admin"), $this->text("clickhere", "pim"), SESSION_NESTED);
      $content = '<br><div align="left" style="width: 400px;">';
      $content.= '<b>' . $this->text("welcome") . '</b><br>';
      $content.= $this->text("loggedinasadmin") . '<br><br>';
      $content.= '<b>' . $this->text("setuppurposesonly") . '</b><br>';
      $content.= $this->text("notarealaccount") . '<br><br>';
      $content.= '<b>' . $this->text("firsttimeusage") . '</b><br>';
      $content.= sprintf($this->text("createfirstrealuseraccount"), $employeeadminlink) . '<br><br>';
      $content.= '<b>' . $this->text("upgradesandinstallations") . '</b><br>';
      $content.= $this->text("usesetuplink") . '<br><br>';
      $content.= '<b>' . $this->text("donations") . '</b><br>';
      $content.= $this->text("becomeasponsor") . '<br><br>';
      $content.= '<a href="http://www.achievo.org/support/donate" target="_new">';
      $content.= '<img border="0" src="modules/pim/paypal_donate.gif" alt="' . $this->text("donate") . '">';
      $content.= '</a>';
      $content.= '</div><br>';

      // Return the generated content
      return $content;
    }

    function action_adminpim(&$handler)
    {
      // Initialize the title and content
      $title = $this->text("title_pim_adminpim");
      $content = $this->getAdminPimContent();

      // Render the page
      $page = &$this->getPage();
      $page->addContent($this->genericPage($title, $content));
    }
  }

?>