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
     *                     "line" - Create a line chart
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

      $obj = &atkGetNode($atknodetype);

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

      if(!is_array($data) OR count($data)==0 OR $data===NULL)
      {
        atkerror("Graph: no graph data?!");
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
      usegraph("bar");
       // We only support one dataset, so we only use the first piece of data.
      $dataset = reset($data);
      $title = key($data);

      // Setup the basic parameters for the graph
      $graphwidth   = atkconfig("graph_width",750);
      $graphheight  = round($graphwidth / 750 * 300);

      $datay=array_values($dataset);
      $datax=array_keys($dataset);


      // Set the basic parameters of the graph
      $graph = new Graph($graphwidth,$graphheight,'auto');
      $graph->SetScale("textlin");

      $top = 100;
      $bottom = 30;
      $left = 70;
      $right = 30;
      $graph->Set90AndMargin($left,$right,$top,$bottom);

      // Nice shadow
      $graph->SetShadow();

      // Setup title
      $graph->title->Set($title);
      $graph->title->SetFont(FF_VERDANA,FS_BOLD,14);
      //$graph->subtitle->Set("(Axis at top)");

      // Setup X-axis
      $graph->xaxis->SetTickLabels($datax);
      $graph->xaxis->SetFont(FF_VERDANA,FS_NORMAL,12);

      // Some extra margin looks nicer
      $graph->xaxis->SetLabelMargin(5);

      // Label align for X-axis
      $graph->xaxis->SetLabelAlign('right','center');

      // Add some grace to y-axis so the bars doesn't go
      // all the way to the end of the plot area
      $graph->yaxis->scale->SetGrace(20);
      $graph->yaxis->SetLabelAlign('center','bottom');
      $graph->yaxis->SetLabelAngle(60);
      $graph->yaxis->SetLabelFormat('%d');
      $graph->yaxis->SetFont(FF_VERDANA,FS_NORMAL,12);

      // We don't want to display Y-axis
      //$graph->yaxis->Hide();

      // Now create a bar pot
      $bplot = new BarPlot($datay);
      $bplot->SetFillColor("red");
      $bplot->SetShadow();

      //You can change the width of the bars if you like
      //$bplot->SetWidth(0.5);

      // We want to display the value of each bar at the top
      $bplot->value->Show();
      $bplot->value->SetFont(FF_ARIAL,FS_BOLD,12);
      $bplot->value->SetAlign('left','center');
      $bplot->value->SetColor("black","darkred");
      $bplot->value->SetFormat(atktext("currencysymbol","atk","","","",true).' %.0f');

      // Add the bar to the graph
      $graph->Add($bplot);


      $graph->Stroke();
      return true;



    }

    function linePlot($data)
    {
      usegraph("line");

      $defcolors = array('red', 'blue', 'green', 'yellow', 'black');

      $graphwidth   = atkconfig("graph_width",750);
      $graphheight  = round($graphwidth / 750 * 300);

      $graph        = new  Graph($graphwidth,$graphheight,"auto");

      $theme = &atkTheme::getInstance();
      $bgColor = $theme->getAttribute("graphBgColor");
      $graph->SetMarginColor($bgColor);


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

      $i=0;
      foreach ($data as $legend=>$values)
      {
        // Create a red line plot
        $p = new LinePlot(array_values($values));
        $col = $defcolors[$i%(count($defcolors))];
        $p->SetColor($col);
        $p->SetLegend(atktext($legend));

        //$p->mark->SetType(MARK_FILLEDCIRCLE);
        $p->mark->SetFillColor($col);
        $p->mark->SetWidth(4);

        $graph->Add($p);
        unset($p);
        $i++;

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

      $graphwidth = atkconfig("graph_width",750);
      $graphheight = round($graphwidth / 750 * 300);

      $graph = new PieGraph($graphwidth,$graphheight,"auto");

      $theme = &atkTheme::getInstance();
      $bgColor = $theme->getAttribute("graphBgColor");
      $graph->SetColor($bgColor);

       // Set title
      $graph->title->Set(atktext($title));

      // Use built in font
      $graph->title->SetFont(FF_FONT1,FS_BOLD);

      // Create a plot
      $p1 = new PiePlot(array_values($dataset));

      $p1->setCenter(0.26,0.5);
      $legends = array_keys($dataset);
      for($i=0,$_i=count($legends);$i<$_i;$i++)
      {
        if(strlen($legends[$i])>45)
          $legends[$i] = substr($legends[$i],0,45)."...";
      }

      $p1->SetLegends($legends);

      $graph->Add($p1);

      // Finally output the  image
      $graph->Stroke();
      return true;
    }
  }

?>