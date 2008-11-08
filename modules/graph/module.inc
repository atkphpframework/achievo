<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage graph
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */

  DEFINE("TTF_DIR",'./modules/graph/fonts/');
  DEFINE("CACHE_DIR",atkconfig('atktempdir').'graph/');

  /**
   * Module file for the menu items and security
   *
   * @package achievo
   * @subpackage graph
   */
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