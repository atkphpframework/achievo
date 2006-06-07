<?php

  /**
   * Global search functionality
   */

  class search extends atkNode
  {
    function search()
    {
      $this->atkNode("search", NF_NO_SECURITY);
      // Anyone may perform global search. (security is enforced per
      // search result)
    }

    function action_search(&$handler)
    {
      global $g_sessionManager;

      // Searchstring is a stackvar. We get it through the post_vars,
      // but we need to store it in the session stack, so the system
      // remembers the last searchstring when coming back from a
      // view action.
      $searchstring = strip_tags($g_sessionManager->stackVar("searchstring"));

      if ($searchstring!="")
      {
        $results = atkHarvestModules("search", $searchstring);
        $firsturl="";
        $res = "";

        foreach ($results as $category=>$items)
        {
          $cnt = count($items);
          if ($cnt>0)
          {
            $hasresults += $cnt;
            $res .= '<br><b>'.atktext($category).'</b><br>';
            for ($i=0;$i<$cnt;$i++)
            {
              $res.= '<br>&nbsp; &nbsp;'.href($items[$i]["url"], $items[$i]["title"], SESSION_NESTED);
              if ($firsturl=="") $firsturl = $items[$i]["url"];
            }
            $res.='<br>';
          }
        }

        if ($hasresults==0)
        {
          $res = '<br>'.sprintf(atktext("search_noresults"), $searchstring);
        }
        else if ($hasresults==1)
        {
          // autoview first url
          return $this->redirect(session_url($firsturl, SESSION_REPLACE));
        }
      }
      else
      {
        $res = '<br>'.atktext("search_emptysearchstring");
      }

      $res.='<br><br>'.atktext('search_again').':<br>';
      $res.=$this->simpleSearchForm($searchstring, "", SESSION_DEFAULT).'<br><br>';

      $ui = &$this->getUi();
      $page = &$this->getPage();
      $this->addStyle("style.css");
      $searchResultBox = $ui->renderBox(array("title"=>atkText("search","search"),"content"=>$res));
      $page->addContent($searchResultBox);
      return true;
    }

    function simpleSearchForm($searchstring, $targetframe="", $sessionstatus=SESSION_NESTED)
    {
      $ui = &$this->getUi();
      $tplvars = array( 'targetframe'=>$targetframe,
                        'sessionform'=>session_form($sessionstatus),
                        'searchstring'=>$searchstring,
                       );
      return $ui->render($ui->templatePath('simplesearch.tpl'),$tplvars);
    }
  }

?>