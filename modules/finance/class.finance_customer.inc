<?php

userelation("atkmanytoonerelation");
userelation("atkonetomanyrelation");

class finance_customer extends atkNode
{
  function finance_customer()
  {
    $this->atkNode("finance_customer", NF_NO_ADD|NF_NO_DELETE); 

    $this->add(new atkAttribute("id" ,AF_AUTOKEY));
    $this->add(new atkAttribute("name",AF_SEARCHABLE|AF_READONLY));
    $this->add(new atkOneToManyRelation("contactperson","contact","company",AF_HIDE_LIST|AF_READONLY));
    $this->add(new atkAttribute("address", AF_HIDE_LIST|AF_READONLY));
    $this->add(new atkAttribute("zipcode", AF_HIDE_LIST|AF_READONLY));
    $this->add(new atkAttribute("city", AF_HIDE_LIST|AF_READONLY));
    $this->add(new atkAttribute("country", AF_HIDE_LIST|AF_READONLY));
    $this->add(new atkAttribute("phone",AF_HIDE_LIST|AF_READONLY));
    $this->add(new atkAttribute("fax",AF_HIDE_LIST|AF_READONLY));
    $this->add(new atkAttribute("email",AF_HIDE_LIST|AF_READONLY));
    $this->add(new atkAttribute("bankaccount", AF_SEARCHABLE));
    $this->add(new atkAttribute("remark", AF_HIDE_LIST|AF_READONLY)); 
    $this->add(new atkManyToOneRelation("currency","finance.currency",AF_SEARCHABLE));
    $this->add(new atkOneToManyRelation("rates","finance.rates","customerid",AF_HIDE_LIST));
    $this->setTable("customer", "customer");
    $this->setOrder("customer.name");
    $this->setIndex("name");
        
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