<?php   

  /* Setup the system */    
  include_once("atk.inc");    

  include_once("theme.inc");
  
  include_once(moduleDir("graph")."jpgraph/jpgraph.php");
    
  atksession("img", false);
  atksecure();
      
  // Create node
  $obj = &getNode($ATK_VARS["atknodetype"]); 
    
  if (is_object($obj))
  {
    // We prepend graph_ to the callback as a security precaution. 
    // This prevents anyone from executing arbitrary methods. 
    // Only methods starting with graph_ can be executed as callback.
    // 
    $method = "graph_".$ATK_VARS["callback"];
    if (method_exists($obj, $method))
    {
      $res = $obj->$method($ATK_VARS);     
    }
    else
    {
      atkerror("Graph: callback $method on source node ".$ATK_VARS["atknodetype"]." does not exist.");
    }
  }
  else
  {
    atkerror("Graph: source node ".$ATK_VARS["atknodetype"]." not found.");
  }
  
  if (!$res)
  {
    $g_layout->outputFlush();
  }
  
?>
