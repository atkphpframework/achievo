<?php

class rates extends atkNode
{
  function rates()
  {
    $this->atkNode("rates");

    $this->addAttribute(new atkAttribute("id" ,AF_PRIMARY|AF_UNIQUE|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkTextAttribute("description"));
    $this->addAttribute(new atkDateAttribute("startdate",AF_OBLIGATORY));
    $this->addAttribute(new atkDateAttribute("enddate",AF_OBLIGATORY));
    $this->addAttribute(new atkAttribute("rate",AF_OBLIGATORY));
    $this->addAttribute(new atkNumberAttribute("priority",AF_OBLIGATORY));
		$this->addAttribute(new atkManytoOneRelation("userid","employee"));
		$this->addAttribute(new atkManytoOneRelation("projectid","project"));
		$this->addAttribute(new atkManytoOneRelation("activityid","activity"));
		$this->addAttribute(new atkManytoOneRelation("customerid","customer"));
		

    $this->setTable("rate","rate");
    $this->setOrder("id");
    
  }

  function initial_values()
	{
	  return array("priority"=>10);
	}
	
	function userid_display($rec)
	{
	  if(is_null($rec["userid"]["userid"]))  { return "-"; } else { return $this->m_attribList["userid"]->display($rec); } 
	}

  function projectid_display($rec)
	{ 
	  if(is_null($rec["projectid"]["id"]))  { return "-"; } else { return $this->m_attribList["projectid"]->display($rec); } 
	}
 
  function activityid_display($rec)
	{
	  if(is_null($rec["activityid"]["id"]))  { return "-"; } else { return $this->m_attribList["activityid"]->display($rec); } 
	}
	
  function customerid_display($rec)
	{
	  if(is_null($rec["customerid"]["id"]))  { return "-"; } else { return $this->m_attribList["customerid"]->display($rec); } 
	}

  function rate_edit($rec)
  {
    return $this->m_attribList["rate"]->edit($rec)." ".atkconfig("currency");
  }
	

  function descriptor_def()
  {
    return "[description] ([rate_id])";
  }

}
?>
