<?php
class billing_project extends atkNode
{
  function billing_project()
  {
    $this->atkNode("billing_project",NF_ADD_LINK);

    $this->addAttribute(new atkAttribute("id" ,AF_PRIMARY|AF_UNIQUE|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkManytoOneRelation("projectid","project"));
    $this->addAttribute(new atkTextAttribute("description"));

    $this->addAttribute(new atkDateAttribute("create_date",AF_OBLIGATORY|AF_READONLY));
    $this->addAttribute(new atkAttribute("userid",AF_OBLIGATORY|AF_READONLY));
    $this->addAttribute(new atkListAttribute("status",array("send","payed"),array("send","payed")));
    $this->addAttribute(new atkBoolAttribute("specify_hours"));

    $this->setTable("bill","bill");
    $this->setOrder("create_date,id");
    
    atkdebug("billing_project::billing_project()");    
  }
  
	function initial_values()
	{
	  global $g_user;
		$initial_date = Array("year"=>date("Y"),
                             "month"=>date("m"),
                             "day"=>date("d"));
														 
	  return array("create_date"=>$initial_date,"userid"=>strtolower($g_user["name"]));
	}
	
	function postUpdate($rec)
	{
	  atkdebug("POST EDIT !!!!!");
		include "./modules/finance/create_bill.inc";
	}

	function postAdd($rec)
	{
	  atkdebug("POST ADD !!!!!");
		include "./modules/finance/create_bill.inc";
	}

}
?>
