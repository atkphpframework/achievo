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
   */
  
  class graphAttribute extends atkAttribute
  {
    var $m_source = "";
    var $m_callback = "";
    var $m_params = array();
    
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
      $this->atkAttribute($name, $flags);      
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
    
    function showGraph($record)
    {
      return '<img src="'.session_url("graph.php?atknodetype=".$this->getSource()."&callback=".$this->m_callback.$this->_getParams($record), SESSION_NEW).'">';
    }
    
    function _getParams($record)
    {
      $res = "";
      
      if (count($this->m_params))
      {
        // Params can use [fieldnames] and [pk]
        $record["pk"] = $this->m_ownerInstance->primaryKey($record);        
        
        foreach($this->m_params as $key=>$value)
        {          
          $res.="&".$key."=".urlencode(stringparse($value, $record));
        }
      }
      return $res;
    }
    
    // Dummy methods to prevent loading/storing of data.
    function load() {}
    function store() {}
    function addToQuery() {}
    function hide() {}
  }

?>