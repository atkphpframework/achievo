<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage pim
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */

  /**
   * Class for rendering the pim page
   *
   * @package achievo
   * @subpackage pim
   */
  class pim extends atkNode
  {
    /**
     * Constructor
     */
    function pim()
    {
      $this->atkNode("pim",NF_NO_SECURITY);
    }

    /**
     * Render action page with all pim items
     *
     * @param object $handler Handler
     */
    function action_pim(&$handler)
    {
      $url = atkconfig::get('pim','pim_redirect_to');
      if ($url != "" && is_array($url))
      {
        $param = atkconfig::get('pim','pim_redirect_to');
        header("Location: " . session_url(dispatch_url($param[0],$param[1])));
        return;
      }

      $pimItems = atkHarvestModules("getPimItems","",true);
      $userpimitems = &atknew("module.pim.userpimitems");
      $user = getuser();
      $userDbItems = $userpimitems->selectDb("userpimitems.userid='".$user["id"]."'","","",array("id","userid"));
      $disallowedpimitems = atkconfig::get("pim","disallowedpimitems", array());
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
            $pimitemid = $modname.'_'.$itemName;
            atkdebug("Check: ".$pimitemid);
            if(in_array($pimitemid, $userItems) && !in_array($pimitemid, $disallowedpimitems))
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
      $content = $this->renderContent(atktext("title_pim"),$res);

      $page->addContent($content);
    }

    /**
     * Get administrator pim content
     *
     * @return string
     */
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
      $content.= '<a href="http://www.achievo.org/services/donate/" target="_new">';
      $content.= '<img border="0" src="modules/pim/paypal_donate.gif" alt="' . $this->text("donate") . '">';
      $content.= '</a>';
      $content.= '</div><br>';

      // Return the generated content
      return $content;
    }

    /**
     * Administrator pim action
     *
     * @param object $handler Handler
     */
    function action_adminpim(&$handler)
    {
      // Initialize the title and content
      $title = $this->text("title_pim_adminpim");
      $content = $this->getAdminPimContent();

      // Render the page
      $page = &$this->getPage();

      $content = $this->renderContent($title,$content);
      $page->addContent($content);
    }

    /**
     * Render content
     *
     * @param string $title Title
     * @param string $content Content
     * @return string
     */
    function renderContent($title,$content)
    {
      $theme = &atkinstance('atk.ui.atktheme');
      if ($theme->getAttribute('useframes',true))
      {
         $renderd_content = $this->genericPage($title, $content);
      }
      else
      {
         $ui = &$this->getUi();

          if (!is_array($content)) $content = array($title=>$content);
          $renderd_content="";
          foreach($content as $itemtitle => $itemcontent)
           $renderd_content.= $ui->renderBox(array("title"=>$itemtitle, "content"=>$itemcontent));
      }
      return $renderd_content;
    }
  }

?>