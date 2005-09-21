<?php   

  /**
   * graph.php
   *
   * This is a wrapper file which produces images for the graph module.
   * You should not need to use this file directly from your code. See the
   * documentation of the graphAttribute and/or dataGraphAttribute attributes
   * in the graph module for instructions how to use the graph module.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   *
   * @version $Revision$
   *
   * $Id$
   *
   * This file is part of Achievo.
   *
   * Achievo is free software; you can redistribute it and/or modify
   * it under the terms of the GNU General Public License as published by
   * the Free Software Foundation; either version 2 of the License, or
   * (at your option) any later version.
   *
   * Achievo is distributed in the hope that it will be useful,
   * but WITHOUT ANY WARRANTY; without even the implied warranty of
   * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   * GNU General Public License for more details.
   *
   */

  /* Setup the system */    
  include_once("atk.inc");    

  include_once("theme.inc");
  
  include_once(moduleDir("graph")."jpgraph/jpgraph.php");
    
  atksession("img", false);
  atksecure();
  
  // Create attribute. 
  $plottersource = $ATK_VARS["plotter"];
  useattrib($plottersource);
  list($module, $attribname) = explode(".", $plottersource);
  
  $plotterclass = $attribname."Plotter";

  $res = false;
  
  if (class_exists($plotterclass))
  {
    // Since php does not support calling static methods
    // on a 'dynamic' class (where the name is inside a
    // variable), we use a companion plotter class to 
    // plot stuff.
    $plotter = &new $plotterclass();
    $res = $plotter->plot($ATK_VARS);
  }
  else
  {  
    atkerror("Graph: plotter ".$plotterclass." not found.");
  }  
  
  if (!$res)
  {
    $output = &atkOutput::getInstance();
    $output->outputFlush();
  }
  
?>