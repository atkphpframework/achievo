<?php

  useattrib("graph.graphattribute");

 /**
   * dataGraphAttribute
   *
   * This is a JpGraph wrapper attribute. It features the possibility to add
   * jpGraph graphs to any node. With the regular attribute flags AF_HIDE_*
   * you can influence when to show the graph. 
   * 
   * This is an extension to the graphAttribute. The graphAttribute allows
   * you to create arbitrary graphs. The datagraph attribute is different 
   * in that it does not allow you to create a graph yourself. It uses a
   * callback to collect a set of data. Depending on several parameters, 
   * a graph is plotted based on the supplied input data.
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
  class dataGraphAttribute extends graphAttribute
  {
    //var $m_graphtype = "auto"; 
  
    /**
     * Constructor.
     *
     * @param $name - The name of the graph (must be unique within the node). 
     *
     * @param $callback - The name of the method to call on the node to 
     *                    collect the data. 
     *                    Example: if you specify "totals" as $callback, the 
     *                    system invokes the method "graph_totals()" in the 
     *                    node. 
     *                    Callback prototype:
     *                    function graph_<callbackname>($params)
     *                    {
     *                    }
     *                    The callback must return an array of data in the 
     *                    following format:
     *                    array('plotname1'=>array('x-axis/legend title'=>
     *                                                     'y-axis/pie value',
     *                                             ...),
     *                          'plotname2'=>...);
     *                    In other words, it must return one or more plots.           
     *
     * @param $params - Specify an array of parameters to be passed to 
     *                  the callback method. It is possible to specify 
     *                  'templates' as values.
     *                  For example, specifying array("selected"=>"[id]" will
     *                  result in a callback invokation with a parameter 
     *                  'selected' which has the id of the current record
     *                  as parameter. A special template is [pk], which 
     *                  passes the entire primary key of the current record.
     *
     * @param $graphtype - The type of graph to plot. Currently supported are:
     *                     "pie" - Create a pie chart
     *                     "bar" - Create a bar chart
     *                     "auto" - Creates a pie chart if only one plot is 
     *                              passed and a bar chart if more than one
     *                              are passed.
     *
     * @param $flags - Any standard attribute (AF_*) flag can be passed.
     *                 As of yet, there are no specific flags for this
     *                 attribute.
     */ 
    function dataGraphAttribute($name, $callback, $params=array(), $graphtype="auto", $flags=0)
    {
      $params["graphtype"] = $graphtype; // treating graphtype as an extra parameter internally saves us a lot of work.
      $this->graphAttribute($name, $callback, $params, $flags);
    }
        
  }
  
  /**
   * Plotter companion class for dataGraphAttribute.
   */
  class dataGraphAttributePlotter 
  {
    /**
     * Plot method.
     * Called by the graph.php image wrapper, to perform the actual plot.
     * 
     * Do not call directly. Internal framework use only, as they say.
     */
    function plot($postvars)
    {    
      $atknodetype = $postvars["atknodetype"];
      $callback    = $postvars["callback"];
      $graphtype   = $postvars["graphtype"];
      
      $obj = &getNode($atknodetype); 
    
      if (is_object($obj))
      {
        // We prepend graph_ to the callback as a security precaution. 
        // This prevents anyone from executing arbitrary methods. 
        // Only methods starting with graph_ can be executed as callback.
        // 
        $method = "graph_".$callback;
        if (method_exists($obj, $method))
        {
          $data = $obj->$method($postvars);
        }
        else
        {
          atkerror("Graph: callback $method on source node ".$atknodetype." does not exist.");
        }
      }
      else
      {
        atkerror("Graph: source node ".$atknodetype." not found.");
        return false;
      }        
      
      // If auto, determine suitable graph type.
      if ($graphtype=="auto" || $graphtype=="")
      {
        if (count($data)==1)
        {
          $graphtype = "pie";
        }
        else
        {
          $graphtype = "bar";
        }
      }
      
      $funcname = $graphtype."Plot";
      return $this->$funcname($data);
                             
    }
    
    function barPlot($data)
    {
    }
    
    function linePlot($data)
    {  
      usegraph("line");
      
      $graph = new  Graph(550,250,"auto");
      
      global $g_theme;      
      $graph->SetMarginColor($g_theme["BgColor"]);
      
      
      // Use an integer X-scale
      $graph->SetScale("textlin");
      
      // Use built in font
      $graph->title->SetFont(FF_FONT1,FS_BOLD);
      
      $xlabels = array_keys(reset($data));
      $graph->xaxis->SetTickLabels($xlabels);
      $graph->xaxis->SetTextTickInterval(1);
      
      // We can place about 5 labels on the x-axis, so we 
      // calculate how big the interval should be.
      // To prevent weird unreadable intervals, we round
      // to the nearest multiple of 5 bigger than what we
      // calculated (to prevent label overlaps).
      $interv = floor(count($xlabels)/5);                   
      $graph->xaxis->SetTextLabelInterval(max(1,$interv+(5-($interv%5))));
      
      foreach ($data as $legend=>$values)
      {
        // Create a red line plot
        $p = new LinePlot(array_values($values));
        $p->SetColor("red");
        $p->SetLegend(text($legend));
        
        $p->mark->SetType(MARK_FILLEDCIRCLE);
        $p->mark->SetFillColor("red");
        $p->mark->SetWidth(4);
        
        $graph->Add($p);                

      }
      
      $graph->Stroke();
      return true;

    }
    
    function piePlot($data)
    {
      usegraph("pie");
    // Determine lookup    
    
      // We only support one dataset, so we only use the first piece of data.
      $dataset = reset($data);
      $title = key($data);

      $graph = new PieGraph(550,250,"auto");
      
      global $g_theme;      
      $graph->SetColor($g_theme["BgColor"]);
      
       // Set title 
      $graph->title->Set(text($title));

      // Use built in font
      $graph->title->SetFont(FF_FONT1,FS_BOLD);

      // Create a plot
      $p1 = new PiePlot(array_values($dataset));
      
      $p1->setCenter(0.26,0.5);
      $p1->SetLegends(array_keys($dataset));
       
      $graph->Add($p1);     

      // Finally output the  image
      $graph->Stroke();    
      return true;
    }
  }
  
?>