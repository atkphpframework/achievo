<?php

userelation("atkmanytoonerelation");
useattrib("atkdateattribute");
useattrib("atknumberattribute");
useattrib("atktextattribute");

class rates extends atkNode
{
  function rates()
  {
    $this->atkNode("rates", NF_ADD_LINK);

    $this->add(new atkAttribute("id" ,AF_AUTOKEY));
    $this->add(new atkTextAttribute("description"));
    $this->add(new atkDateAttribute("startdate",AF_OBLIGATORY));
    $this->add(new atkDateAttribute("enddate",AF_OBLIGATORY));
    $this->add(new atkAttribute("rate",AF_OBLIGATORY));
    $this->add(new atkNumberAttribute("priority",AF_OBLIGATORY));
		$this->add(new atkManytoOneRelation("userid","employee.employee", AF_RELATION_AUTOLINK));
		$this->add(new atkManytoOneRelation("projectid","project.project", AF_RELATION_AUTOLINK));
		$this->add(new atkManytoOneRelation("activityid","project.activity", AF_RELATION_AUTOLINK));
		$this->add(new atkManytoOneRelation("customerid","organization.customer", AF_RELATION_AUTOLINK));		

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