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

  function project_id_display($rec)
	{ 
	  if(is_null($rec["project_id"]["id"]))  { return "-"; } else { return $this->m_attribList["project_id"]->display($rec); } 
	}
 
  function activity_id_display($rec)
	{
	  if(is_null($rec["activity_id"]["id"]))  { return "-"; } else { return $this->m_attribList["activity_id"]->display($rec); } 
	}
	
  function customer_id_display($rec)
	{
	  if(is_null($rec["customer_id"]["id"]))  { return "-"; } else { return $this->m_attribList["customer_id"]->display($rec); } 
	}
	

  function descriptor_def()
  {
    return "[description] ([rate_id])";
  }

}
?>
