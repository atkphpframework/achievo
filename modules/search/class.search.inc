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
    
    function action_search()
    {
      global $g_layout, $g_sessionManager;

      $g_layout->initGui();
      $g_layout->ui_top(text("search"));  
      
      // Searchstring is a stackvar. We get it through the post_vars, 
      // but we need to store it in the session stack, so the system
      // remembers the last searchstring when coming back from a 
      // view action.
      $searchstring = $g_sessionManager->stackVar("searchstring");
      
      if ($searchstring!="")
      {  
        $results = atkHarvestModules("search", $searchstring);
    
        $res = "";
        $hasresults = false; // We can't use count($results) to determine if we 
                             // found anything. Bad modules might set a category
                             // in $results even if they haven't got results in
                             // that category.
        
        foreach ($results as $category=>$items)
        {
          $cnt = count($items);
          if ($cnt>0)
          {
            $hasresults = true;
            $res .= '<br><b>'.text($category).'</b><br>';
            for ($i=0;$i<$cnt;$i++)
            {        
              $res.= '<br>&nbsp; &nbsp;'.href($items[$i]["url"], $items[$i]["title"], SESSION_NESTED);
            }
            $res.='<br>';
          }
        }      
      
        if (!$hasresults)
        {  
          $res = '<br>'.sprintf(text("search_noresults"), $searchstring);
        }  
      }
      else
      {       
        $res = '<br>'.text("search_emptysearchstring");       
      }
      
      $g_layout->output($res.'<br><br>'.text('search_again').':<br>');
      $g_layout->output($this->simpleSearchForm($searchstring, "", SESSION_DEFAULT).'<br><br>');
      
      $g_layout->ui_bottom();
    }
    
    function simpleSearchForm($searchstring, $targetframe="", $sessionstatus=SESSION_NESTED)
    {
      $res = '<form action="dispatch.php"'.($targetframe!=""?'target="'.$targetframe.'"':'').'>';
      $res.= '<input type="hidden" name="atknodetype" value="search.search">';
      $res.= '<input type="hidden" name="atkaction" value="search">';
      $res.= session_form($sessionstatus);
      $res.= '<input name="searchstring" type="text" size="18" value="'.$searchstring.'">&nbsp;<input type="submit" value="'.text("search").'">';
      $res.= '</form>';
      return $res;
    }
  }
  
?>