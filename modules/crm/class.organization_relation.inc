<?PHP
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage crm
 *
 * @copyright (c)2008 Sandy Pleyte
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */
userelation("atkmanytoonerelation");

/**
 * Class for managing organization relations
 * 
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage crm
 * @since 1.3.0
 */
class organization_relation extends atkNode
{

    /**
     * Constructor
     *
     * @return void
     */
    function organization_relation()
    {
        $this->atkNode("organization_relation");

        $this->add(new atkAttribute("relation_id", AF_AUTOKEY));
        $this->add(new atkManyToOneRelation("account", "organization.organization", AF_HIDE));

        $this->add(new atkManyToOneRelation("relation_with", "organization.organization", AF_LARGE | AF_OBLIGATORY));
        $this->add(new atkManyToOneRelation("kind_of", "crm.relation_type", AF_OBLIGATORY));

        $this->setSecurityAlias("organization.organization");
        $this->setTable("crm_organization_relation");
    }

}

?>