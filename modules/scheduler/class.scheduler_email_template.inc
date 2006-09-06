<?php

atkimport("modules.email_template.email_template");
useattrib("atkdummyattribute");
class scheduler_email_template extends email_template
{
  function scheduler_email_template()
  {
    $this->email_template("scheduler_email_template");
    $this->addFilter("email_template.module='scheduler'");
  }

  function getValidCodes()
  {
    return array("title","location","description");
  }
}
?>