<?php


class atkHiddenAttribute extends atkDummyAttribute 
{
	var $m_value = "";
	
	function atkHiddenAttribute($name, $value="", $flags=0)
	{
		$this->m_value = $value;
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

}

?>