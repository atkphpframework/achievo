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

  useattrib("atkattribute");

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
   * @package achievo
   * @subpackage graph
   */
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

    /**
     * Add dynamic params
     *
     * @param string $paramname Param name
     * @param mixed $possiblevalues Value(s)
     */
    function addDynamicParam($paramname, $possiblevalues)
    {
      $this->m_dynparams[$paramname] = $possiblevalues;
    }

    /**
     * Return the source node responsible for drawing the graph.
     * Usually, this is the node to which the attribute was added, unless
     * setSource was used to specify a different node.
     * @return string Source
     */
    function getSource()
    {
      return ($this->m_source==""?$this->m_ownerInstance->atkNodeType():$this->m_source);
    }

    /**
     * Show graph when we edit this field
     *
     * @param array $record Record
     * @param string $prefix Prefix
     * @param string $mode Mode
     * @return string
     */
    function edit($record, $prefix, $mode)
    {
      return $this->showGraph($record, false);
    }

    /**
     * Show graph when we display this field
     *
     * @param array $record Record
     * @param string $mode Mode
     * @return string
     */
    function display($record, $mode)
    {
      return $this->showGraph($record, false);
    }

    /**
     * Show graph
     *
     * @param array $record Record
     * @param boolean $addForm Add to form
     * @return string
     */
    function showGraph($record=array(), $addForm=true)
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
        if ($addForm) $res.='<form name="dummyform">';
        foreach ($this->m_dynparams as $paramname=>$possiblevalues)
        {
          $res.= '<br />'.atktext($paramname).": ";

          for ($i=0, $_i=count($possiblevalues); $i<$_i; $i++)
          {
            $checked = ($possiblevalues[$i]==$this->m_params[$paramname]?"checked":"");
            $res.= '<input type="radio" name="resolution" onClick="';
            $res.= $imgname."_".$paramname."='".$possiblevalues[$i]."'; ".$imgname."_refresh();";
            $res.= '" value="'.$possiblevalues[$i].'" '.$checked.'>'.
                        atktext($possiblevalues[$i]);
          }
        }
        if ($addForm) $res.='</form>';
      }

      return $res;
    }

    /**
     * Graph url
     *
     * @param array $record Record
     * @return string
     * @access private
     */
    function _graphUrl($record=array())
    {
      return $this->_baseUrl().$this->_paramsToUrl($record);
    }

    /**
     * Get base url
     *
     * @return string
     * @access private
     */
    function _baseUrl()
    {
      return session_url("graph.php?atknodetype=".$this->getSource().
                         "&plotter=graph.".get_class($this).
                         "&callback=".$this->m_callback, SESSION_NEW);
    }

    /**
     * Convert params to url
     *
     * @param array $record Record
     * @return string
     * @access private
     */
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
          if(is_array($value))
          {
            foreach($value as $value_key=>$value_value)
            {
              $res.="&".$key."%5B".$value_key."%5D=".urlencode($value_value);
            }
          }
          else
          {
            $res.="&".$key."=".urlencode(stringparse($value, $record));
          }
        }
      }
      return $res;
    }

    /**
     * Parse params
     *
     * @param array $record
     * @return array
     * @access private
     */
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
    function storageType() { return NOSTORE; }
    function loadType() { return POSTLOAD; }
  }

  /**
   * Plotter companion class for graphAttribute.
   * @package achievo
   * @subpackage graph
   */
  class graphAttributePlotter
  {
    /**
     * Plot method.
     * Called by the graph.php image wrapper, to perform the actual plot.
     *
     * Do not call directly. Internal framework use only, as they say.
     * @param array $postvars Postvars
     * @return string
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
