<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage person
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */
useattrib("atknumberattribute");

/**
 * The node class for managing titles
 * @package achievo
 * @subpackage person
 */
class title extends atkNode
{

    /**
     * Constructor
     */
    function title()
    {
        $this->atkNode("title");
        $this->add(new atkNumberAttribute("id", AF_AUTOKEY));

        $this->add(new atkAttribute("title", 0, 25));

        $this->setTable("title");
        $this->setSecurityAlias("organization.contact");
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
        return "[title]";
    }

}

?>
