<?php

  class mod_graph extends achievoModule
  {    
  }
  
  /**
   * Function to include a certain jpGraph graph plotter.
   * Similar to useattrib() and userelation() this method is used when you
   * want to draw a certain jpGraph graph. 
   *
   * @param $graphname - The name of the graph to use. The method will look
   *                     for a jpgraph_$graphname.php file in the
   *                     modules/graph/jpgraph/ directory. Take a look in this
   *                     directory to see what graphs can be used (not all 
   *                     graphs are included in Achievo).
   */
  function usegraph($graphname)
  {
    include_once(moduleDir("graph")."jpgraph/jpgraph_$graphname.php");
  }

?>