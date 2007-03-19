<?php

useattrib("atkdateattribute");
useattrib("atktextattribute");
useattrib("atkcurrencyattribute");
useattrib("atkUpdateStampAttribute");
userelation("atkmanytoonerelation");
 useattrib("atkcreatedbyattribute");
  useattrib("atkCreateStampAttribute");
  useattrib("atkupdatedbyattribute");
  useattrib("atkupdatestampattribute");

class campaign extends atkNode 
{
  
  function campaign()
  {
    $this->atkNode("campaign",NF_MRA|NF_ADD_LINK);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("title",AF_OBLIGATORY|AF_SEARCHABLE));
    $this->add(new atkManyToOneRelation("type","organization.campaign_type",AF_SEARCHABLE));
    $this->add(new atkManyToOneRelation("status","organization.campaign_status",AF_SEARCHABLE));
    $this->add(new atkDateAttribute("start_date"));
    $this->add(new atkDateAttribute("end_date"));
    $this->add(new atkCurrencyAttribute("budget",AF_HIDE_LIST));
    $this->add(new atkCurrencyAttribute("actual_cost",AF_HIDE_LIST));
    $this->add(new atkCurrencyAttribute("expected_revenue",AF_HIDE_LIST));
    $this->add(new atkCurrencyAttribute("expected_cost",AF_HIDE_LIST));
    $this->add(new atkTextAttribute("objective",AF_HIDE_LIST));
    $this->add(new atkTextAttribute("description",AF_HIDE_LIST));
    
    $this->add(new atkCreatedByAttribute("created_by",AF_HIDE_LIST),"history");
    $this->add(new atkCreateStampAttribute("created_on",AF_HIDE_LIST),"history");
    $this->add(new atkUpdatedByAttribute("last_modified_by",AF_HIDE_LIST),"history");
    $this->add(new atkUpdateStampAttribute("last_modified_on",AF_HIDE_LIST),"history");
    
    
    $this->setSecurityAlias("organization.organization");
    $this->setOrder("title");
    $this->setTable("campaign");
  }
  

  
  
  function descriptor_def()
  {
    return "[title]";
  }
}



?>