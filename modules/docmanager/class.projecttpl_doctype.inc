<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 * projecttpl_doctype node class file
 *
 * @package achievo
 * @subpackage docmanager
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */
/**
 * @internal includes
 */
userelation("atkmanytoonerelation");

/**
 * The node class for projecttpl_doctype.
 *
 * @author ivo <ivo@ibuildings.nl>
 * @author guido <guido@ibuildings.nl>
 * @package achievo_modules
 * @subpackage docmanager
 */
class projecttpl_doctype extends atkNode
{

    /**
     * projecttpl_doctype constructor
     */
    function projecttpl_doctype()
    {
        // Call the base class constructor
        $this->atkNode("projecttpl_doctype");

        // Add the relations to both the tpl_project and the documenttype tables
        $this->add(new atkManyToOneRelation("projecttpl_id", "project.tpl_project", AF_PRIMARY));
        $this->add(new atkManyToOneRelation("documenttype_id", "docmanager.documenttype", AF_PRIMARY));

        // Set the table for this node
        $this->setTable("docmanager_prjtpl_dt");
    }

}

?>