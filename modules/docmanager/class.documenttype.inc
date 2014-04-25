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
// Load used classes
useattrib("docmanager.documentfileattribute");
useattrib("atklistattribute");

/**
 * The node class for documenttype.
 *
 * @author guido <guido@ibuildings.nl>
 * @package achievo_modules
 * @subpackage docmanager
 */
class documenttype extends atkNode
{

    /**
     * documenttype constructor
     */
    function documenttype()
    {
        $this->atkNode("documenttype");
        $this->add(new atkAttribute("id", AF_AUTOKEY));
        $this->add(new atkAttribute("name", AF_OBLIGATORY));
        $this->add(new documentFileAttribute("template", atkconfig::get("docmanager", "docmanager_doctypetemplatepath", moduledir("docmanager") . "doctypetemplates/"), atkconfig::get("docmanager", "docmanager_localdocumenttypepath"), false, AF_OBLIGATORY));

        $nodes = atkconfig::get("docmanager", "docmanagernodes", array());
        $nodes[] = "project.project_personcontact";
        $nodetranslations = array();
        foreach ($nodes as $node) {
            $nodetranslations[] = atktext(getNodeType($node), getNodeModule($node));
        }
        $this->add(new atkListAttribute("master", $nodetranslations, $nodes, AF_OBLIGATORY | AF_NO_TRANSLATION | AF_FILE_NO_SELECT, 255));

        $this->setTable("docmanager_documenttype");
    }

    /**
     * Return record descriptor (for use in relations etc.)
     *
     * @return String A template containing fieldnames between brackets.
     */
    function descriptor_def()
    {
        return "[name]";
    }

}

?>