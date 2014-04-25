<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage employee
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */
userelation("atkmanytoonerelation");

/**
 * Class for managing employee roles
 * @package achievo
 * @subpackage employee
 */
class employeerole extends atkNode
{

    /**
     * Constructor
     */
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
     * @todo: replace this with proper descriptor fields.
     */
    function descriptor_def()
    {
        return "[id]";
    }

}

?>