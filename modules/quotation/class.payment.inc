<?php

class payment extends atkNode
{
	function payment()
	{
		$this->atkNode("quotation_payment");
		$this->add(new atkAttribute("id", AF_AUTOKEY));
		$this->add(new atkAttribute("type", AF_UNIQUE|AF_OBLIGATORY|AF_SEARCHABLE));
		$this->setTable("quotation_payment");
	}

	function descriptor_def()
  {
    return "[type]";
  }

}

?>