<?php

class finance_customer extends atkNode
{
  function finance_customer()
  {
    $this->atkNode("finance_customer", NF_NO_ADD|NF_NO_DELETE); 

    $this->addAttribute(new atkAttribute("id" ,AF_HIDE|AF_AUTO_INCREMENT|AF_PRIMARY));
    $this->addAttribute(new atkAttribute("name",AF_SEARCHABLE|AF_OBLIGATORY|AF_READONLY));
    $this->addAttribute(new atkOneToManyRelation("contactperson","contact","company",AF_HIDE_LIST|AF_READONLY));
    $this->addAttribute(new atkAttribute("address", AF_HIDE_LIST|AF_READONLY));
    $this->addAttribute(new atkAttribute("zipcode", AF_HIDE_LIST|AF_READONLY));
    $this->addAttribute(new atkAttribute("city", AF_HIDE_LIST|AF_READONLY));
    $this->addAttribute(new atkAttribute("country", AF_HIDE_LIST|AF_READONLY));
    $this->addAttribute(new atkAttribute("phone",AF_HIDE_LIST|AF_READONLY));
    $this->addAttribute(new atkAttribute("fax",AF_HIDE_LIST|AF_READONLY));
    $this->addAttribute(new atkAttribute("email",AF_HIDE_LIST|AF_READONLY));
    $this->addAttribute(new atkAttribute("bankaccount", AF_SEARCHABLE));
    $this->addAttribute(new atkTextAttribute("remark", AF_HIDE_LIST|AF_READONLY)); 
    $this->addAttribute(new atkManyToOneRelation("currency","finance.currency",AF_SEARCHABLE));
    $this->addAttribute(new atkOneToManyRelation("rates","finance.rates","customerid",AF_HIDE_LIST));
    $this->setTable("customer", "customer");
    $this->setOrder("customer.name");
    $this->setIndex("name");
    
    atkdebug("finance_customer::finance_customer()");    
  }

  function email_display($record)
  {
    $email = $record["email"];
    if (strlen($email)>0) return '<a href="mailto:'.$email.'">'.$email.'</a>';
    return "";
  }
	
	function currency_display($rec)
	{
	  //var_dump($rec);
		//echo "<hr>";
	  if(is_null($rec["currency"]["name"]))  
		{ 
		  return atkconfig("currency")." (".atkconfig("currency_symbol").")";
	  }
		else 
		{ 
		  return $this->m_attribList["currency"]->display($rec); 
		} 
	}

  function descriptor_def()
  {
    return "[name], [city]";
  }
}
?>
