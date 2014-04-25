<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
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
// Add the docmanager to any modules specified in the $config_docmanagernodes array
if (!is_array($modifiers))
    $modifiers = array();
$modifiers = array_values(array_diff(array_merge($modifiers, atkconfig::get("docmanager", "docmanagernodes", array())), array("project.project")));
if (moduleExists('project')) {
    $modifiers[] = "project.tpl_project";
    $modifiers[] = "project.project";
    $modifiers[] = "project.project_personcontact";
}
if (moduleExists('quotation'))
    $modifiers[] = "quotation.quotation";

/**
 * Module file for the menu items and security
 *
 * @author guido <guido@ibuildings.nl>
 * @package achievo_modules
 * @subpackage docmanager
 * @since 1.3.0
 */
class mod_docmanager extends achievoModule
{

    /**
     * Sets the nodes contained within this module and the actions a user could perform on them
     */
    function getNodes()
    {
        registerNode("docmanager.document", array("add", "edit", "delete", "generate"));
        registerNode("docmanager.documenttype", array("admin", "add", "edit", "delete"));
    }

    /**
     * Adds menu items to the menu when loading the menu
     */
    function getMenuItems()
    {
        $this->menuitem("document_types", dispatch_url("docmanager.documenttype", "admin"), "setup", array("docmanager.documenttype", "admin"));
    }

    /**
     * Generic modifier for nodes that want to include the documentManager attribute
     *
     * @param atkNode &$node Node
     */
    function addDocumentManager(&$node)
    {
        useattrib("docmanager.documentmanager");
        $node->add(new documentManager("documents", AF_HIDE_LIST | AF_HIDE_SEARCH), "documents");
    }

    /**
     * Get modifier functions
     *
     * @param atkNode $node Node
     * @return array
     */
    function getModifierFunctions(&$node)
    {
        $modifierfunctions = parent::getModifierFunctions($node);
        $docmanagernodes = atkconfig::get("docmanager", "docmanagernodes", array());
        if (in_array($node->atknodetype(), $docmanagernodes) && ($node->atknodetype() != "project.project"))
            $modifierfunctions[] = "addDocumentManager";
        return $modifierfunctions;
    }

    /**
     * Modifier on project.project node
     *
     * @param atkNode &$node Node
     */
    function project_tpl_project_modifier($node)
    {
        userelation("atkmanyboolrelation");
        $rel = &$node->add(new atkManyBoolRelation("documenttypes", "docmanager.projecttpl_doctype", "docmanager.documenttype", AF_HIDE_LIST | AF_HIDE_ADD));
        $rel->setLocalKey("projecttpl_id");
        $rel->setRemoteKey("documenttype_id");
        $rel->setDestinationFilter("master='project.project'");
        $node->add($rel);
    }

    function project_project_modifier($node)
    {
        useattrib("docmanager.projectdocumentmanager");
        $node->add(new projectDocumentManager("documents", AF_HIDE_LIST | AF_HIDE_SEARCH), "documents");
    }

    /**
     * Modifier on  Project person contact node
     * @param atkNode $node Node
     */
    function project_project_personcontact_modifier($node)
    {
        useattrib("docmanager.relateddocument");
        $node->add(new relatedDocument("document", "projectid"));
    }

    /**
     * Modifuer on Quotation node
     * @param atkNode $node node
     */
    function quotation_quotation_modifier($node)
    {
        $this->addDocumentManager($node);
    }

}
