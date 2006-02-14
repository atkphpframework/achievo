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
        $resultcnt = 0;  // We can't use count($results) to determine if we
                         // found anything. Bad modules might set a category
                         // in $results even if they haven't got results in
                         // that category.

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
      atkimport("atk.layout");
      $layout = &layout::getInstance();

      $layout->initGui();
      $layout->ui_top(atktext("search"));
      $layout->output($res.'<br><br>'.atktext('search_again').':<br>');
      $layout->output($this->simpleSearchForm($searchstring, "", SESSION_DEFAULT).'<br><br>');
      $layout->ui_bottom();
    }

    function simpleSearchForm($searchstring, $targetframe="", $sessionstatus=SESSION_NESTED)
    {
      $res = '<form action="dispatch.php"'.($targetframe!=""?'target="'.$targetframe.'"':'').'>';
      $res.= '<input type="hidden" name="atknodetype" value="search.search">';
      $res.= '<input type="hidden" name="atkaction" value="search">';
      $res.= session_form($sessionstatus);
      $res.= '<input name="searchstring" type="text" size="18" value="'.$searchstring.'">&nbsp;<input type="submit" value="'.atktext("search").'">';
      $res.= '</form>';
      return $res;
    }
  }

?>