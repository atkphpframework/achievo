<?php

  userelation("atkmanytoonerelation");

  /**
   * The node class for employeerole.
   */
  class employeerole extends atkNode
  {
    function employeerole()
    {
      $this->atkNode("employeerole", NF_NO_EDIT);
      $this->add(new atkManyToOneRelation("employee_id", "employee.employee", AF_PRIMARY));
      $this->add(new atkManyToOneRelation("role_id", "employee.profile", AF_PRIMARY));      
      
      $this->setTable("employeerole");
    }
    
    /**
     * Set up default values for this node
     * @return array Associative array where the key is
     *               the attribute name, and value is the
     *               default value.
     */
    function initial_values()
    {    
      return array();
    }
    
    /**
     * Return record descriptor (for use in relations etc.)
     * @return String A template containing fieldnames between brackets.
     */
    function descriptor_def()
    {
      // @todo: replace this with proper descriptor fields.
      return "[id]";
    }
  }

?>