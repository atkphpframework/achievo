<?php


class atkHiddenAttribute extends atkDummyAttribute 
{
	var $m_value = "";
	
	function atkHiddenAttribute($name, $value="", $flags=0)
	{
		$this->m_value = $value;		
		//$this->atkAttribute($name, $flags, 0); // base class constructor
		$this->atkDummyAttribute($name, "", $flags); // base class constructor
	}
	
  function edit()
  {
  	return "<input type=\"hidden\" id=\"$this->m_name\" value=\"$this->m_value\" name=\"$this->m_name\" />";
  }
  
 	function hide()
 	{
 		return $this->edit();
 	} 	
 	
// 	function hasStore($mode)
// 	{
// 		return false; 		
// 	}
 	
// 	function store()
// 	{
// 		//do nothing
// 	}
 	
// 	function load()
// 	{
// 		//do nothing
// 	}
}

?>