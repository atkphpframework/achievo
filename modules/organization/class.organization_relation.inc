<?PHP
userelation("atkmanytoonerelation");
class organization_relation extends atkNode 
{
  function organization_relation()
  {
    $this->atkNode("organization_relation");

    $this->add(new atkAttribute("relation_id",AF_AUTOKEY));
    $this->add(new atkManyToOneRelation("account","organization.organization",AF_HIDE));
    
    $this->add(new atkManyToOneRelation("relation_with","organization.organization",AF_LARGE|AF_OBLIGATORY));
    $this->add(new atkManyToOneRelation("kind_of","organization.relation_type",AF_OBLIGATORY));

    $this->setSecurityAlias("organization.organization");
    $this->setTable("organization_relation", "organization_relation");

  }
  
  
  
}

?>