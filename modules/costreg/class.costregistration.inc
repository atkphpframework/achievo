<?php

  useattrib("project.projectattrib");
  useattrib("atkDateAttribute");
  useattrib("atkListAttribute");
  userelation("atkManyToOneRelation");

  class costregistration extends atkNode
  {
    function costregistration()
    {
      global $g_user, $config_currency_symbol, $atkaction;

      $this->atkNode("costregistration", NF_MRA); // node() constructor is *not* called automatically!

      if($atkaction!='costreport')
      {
        $this->addFilter("userid",strtolower($g_user["name"]));
      }

      $this->add(new atkAttribute("id", AF_PRIMARY|AF_UNIQUE|AF_AUTO_INCREMENT|AF_HIDE));
      $this->add(new atkAttribute("userid", AF_READONLY|AF_HIDE));
      $this->add(new atkAttribute("description", AF_OBLIGATORY|AF_SEARCHABLE));
      $this->add(new atkDateAttribute("costdate","F d Y","d F Y", 0, date("Ymd"),AF_OBLIGATORY));
      $this->add(new atkProjectAttribute("projectid", AF_OBLIGATORY|AF_FORMSUBMIT));

      $this->add(new atkAttribute("value", AF_OBLIGATORY));
      $this->add(new atkAttribute("description", AF_OBLIGATORY));
      $this->add(new atkManyToOneRelation("currency","finance.currency", AF_RELATION_AUTOLINK));
      $this->add(new atkListAttribute("paymethod",array("cash","electronic payment","creditcard"), array("cash","electronic payment","creditcard"), AF_OBLIGATORY));

      $this->setOrder("costdate");

      $this->setTable("costregistration", "costregistration");
    }

    function initial_values()
    {
      global $g_user;
      return array("userid"=>strtolower($g_user["name"]));
    }

    function value_display($rec)
    {
      $value = $rec["value"];
      $value = round($value, 2);
      $value =  number_format($value, 2, '.', '');
      return "$value";
    }

    function currency_display($rec)
    {
      global $config_currency_symbol;

      if ($rec["currency"]["name"] == "") {
        $cur = $config_currency_symbol;
      }
      else {
        $cur = $rec["currency"]["name"];
      }
      return "$cur";
    }

    function action_costreport()
    {
      include moduleDir("costreg").'costreport.inc';
    }

    function descriptor_def()
    {
      return "[description]";
    }
  }

?>