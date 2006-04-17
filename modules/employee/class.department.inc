<?php
useattrib("atkNumberAttribute");
useattrib("atkTextAttribute");

class department extends atkNode
{
  function department()
  {
    $this->atkNode("department",NF_AUTOSELECT);
    $this->add(new atkNumberAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("name",AF_OBLIGATORY));
    $this->add(new atkTextAttribute("description"));
    $this->setOrder("name");
    $this->setTable("employee_department");
  }

  function descriptor_def()
  {
    return "[name]";
  }
}

?>