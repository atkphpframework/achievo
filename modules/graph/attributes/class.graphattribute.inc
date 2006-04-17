<?php

  /**
   * graphAttribute
   *
   * This is a JpGraph wrapper attribute. It features the possibility to add
   * jpGraph graphs to any node. With the regular attribute flags AF_HIDE_*
   * you can influence when to show the graph.
   *
   * All it takes to add a graph to a node is to add this attribute, and
   * define a callback method in the node that collects the data and draws
   * the actual graph.
   * See the constructor method for instructions of how to add the attribute
   * to a node.
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

  useattrib("atkattribute");

  class graphAttribute extends atkAttribute
  {
    var $m_source = "";
    var $m_callback = "";
    var $m_params = array();
    var $m_dynparams = array();

    /**
     * Constructor.
     *
     * @param $name - The name of the graph (must be unique within the node).
     *
     * @param $callback - The name of the method to call on the node to
     *                    collect the data and draw the graph.
     *                    Example: if you specify "totals" as $callback, the
     *                    system invokes the method "graph_totals()" in the
     *                    node.
     *                    Callback prototype:
     *                    function graph_<callbackname>($params)
     *                    {
     *                    }
     *                    The callback must return true if it was able to
     *                    generate a graph. False if something went wrong.
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
     * @param $flags - Any standard attribute (AF_*) flag can be passed.
     *                 As of yet, there are no specific flags for this
     *                 attribute.
     */
    function graphAttribute($name, $callback, $params=array(), $flags=0)
    {
      $this->atkAttribute($name, $flags|AF_HIDE_SEARCH);
      $this->m_callback = $callback;
      $this->m_params = $params;
    }

    /**
     * Set the source node that is responsible for drawing the graph.
     *
     * By default, the callback method is invoked on the node to which the
     * attributewas added. By using setSource on the attribute, you can
     * specify a different node to generate the graph.
     *
     * If the graphAttribute is used outside a standard node context (for
     * example, it's instantiated stand-alone), setSource should always be
     * called to tell the attribute which node to use to generate the graph.
     *
     * @param $node - A valid modulename.nodename combination.
     */
    function setSource($node)
    {
      $this->m_source = $node;
    }

    function addDynamicParam($paramname, $possiblevalues)
    {
      $this->m_dynparams[$paramname] = $possiblevalues;
    }

    /**
     * Return the source node responsible for drawing the graph.
     * Usually, this is the node to which the attribute was added, unless
     * setSource was used to specify a different node.
     */
    function getSource()
    {
      return ($this->m_source==""?$this->m_ownerInstance->atkNodeType():$this->m_source);
    }

    function edit($record, $prefix, $mode)
    {
      return $this->showGraph($record);
    }

    function display($record, $mode)
    {
      return $this->showGraph($record);
    }

    function showGraph($record=array())
    {
      $imgname = $this->fieldName();
      $res = '<img name="'.$imgname.'" src="'.$this->_graphUrl($record).'">';
      if (count($this->m_dynparams))
      {
        $res.="<script language=\"javascript\">\n";

        $decodedparams = $this->_parseParams($record);

        $paramelems = "";
        foreach ($decodedparams as $paramname=>$value)
        {
          $res.= $imgname."_".$paramname." = '".$value."';\n";
          $paramelems[]= $paramname."='+".$imgname."_".$paramname;
        }

        $res.=" function ".$imgname."_refresh()
                {
                  document.".$imgname.".src = '".$this->_baseUrl().implode("+'&", $paramelems).";
                }\n";

        $res.='</script>';
        $res.='<form name="dummyform">';
        foreach ($this->m_dynparams as $paramname=>$possiblevalues)
        {
          $res.=atktext($paramname).": ";

          for ($i=0, $_i=count($possiblevalues); $i<$_i; $i++)
          {
            $checked = ($possiblevalues[$i]==$this->m_params[$paramname]?"checked":"");
            $res.= '<input type="radio" name="resolution" onClick="';
            $res.= $imgname."_".$paramname."='".$possiblevalues[$i]."'; ".$imgname."_refresh();";
            $res.= '" value="'.$possiblevalues[$i].'" '.$checked.'>'.
                        atktext($possiblevalues[$i]);
          }
        }
        $res.='</form>';
      }

      return $res;
    }

    function _graphUrl($record=array())
    {
      return $this->_baseUrl().$this->_paramsToUrl($record);
    }

    function _baseUrl()
    {
      return session_url("graph.php?atknodetype=".$this->getSource().
                         "&plotter=graph.".get_class($this).
                         "&callback=".$this->m_callback, SESSION_NEW);
    }

    function _paramsToUrl($record=array())
    {
      $res = "";

      $decodedparams = $this->_parseParams($record);

      if (count($decodedparams))
      {
        // Params can use [fieldnames] and [pk]
        if (is_object($this->m_ownerInstance))
        {
          $record["pk"] = $this->m_ownerInstance->primaryKey($record);
        }
        foreach($decodedparams as $key=>$value)
        {
          $res.="&".$key."=".urlencode(stringparse($value, $record));
        }
      }
      return $res;
    }

    function _parseParams($record=array())
    {
      $res = array();

      if (count($this->m_params))
      {
        // Params can use [fieldnames] and [pk]
        if (is_object($this->m_ownerInstance))
        {
          $record["pk"] = $this->m_ownerInstance->primaryKey($record);
        }
        foreach($this->m_params as $key=>$value)
        {
          $res[$key] = stringparse($value, $record);
        }
      }
      return $res;
    }

    // Dummy methods to prevent loading/storing of data.
    function load() {}
    function store() {}
    function addToQuery() {}
    function dbFieldType() {}
    function hide() {}
    function search() {}
    function getSearchModes() {}
    function searchCondition() {}
  }

  /**
   * Plotter companion class for graphAttribute.
   */
  class graphAttributePlotter
  {
    /**
     * Plot method.
     * Called by the graph.php image wrapper, to perform the actual plot.
     *
     * Do not call directly. Internal framework use only, as they say.
     */
    function plot($postvars)
    {
      $res = false;

      // Create node
      $atknodetype = $postvars["atknodetype"];
      $callback = $postvars["callback"];
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
          $res = $obj->$method($postvars);
        }
        else
        {
          atkerror("Graph: callback $method on source node ".$atknodetype." does not exist.");
        }
      }
      else
      {
        atkerror("Graph: source node ".$atknodetype." not found.");
      }
      return $res;

    }
  }

?>
