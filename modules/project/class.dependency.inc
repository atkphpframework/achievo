<?php

  userelation("atkmanytoonerelation");
  
  class dependency extends atkNode
  {
    function dependency()
    {
      // Dummy readonly class for accessing dependency information.
      // For editing dependencies, use the dependency attribute.
      $this->atkNode("dependency", NF_READONLY);
      
      $this->add(new atkManyToOneRelation("phaseid_row", "project.phase", AF_PRIMARY));
      $this->add(new atkManyToOneRelation("phaseid_col", "project.phase", AF_PRIMARY));

      $this->setTable("dependency");
    }
  }
?>