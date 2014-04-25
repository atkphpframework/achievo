<?php
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
useattrib("atkdateattribute");
useattrib("atktextattribute");
useattrib("atkcurrencyattribute");
useattrib("atkUpdateStampAttribute");
userelation("atkmanytoonerelation");
useattrib("atkcreatedbyattribute");
useattrib("atkCreateStampAttribute");
useattrib("atkupdatedbyattribute");
useattrib("atkupdatestampattribute");

/**
 * Class for managing activities
 * 
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage crm
 * @since 1.3.0
 */
class campaign extends atkNode
{

    /**
     * Constructor
     *
     * @return void
     */
    function campaign()
    {
        $this->atkNode("campaign", NF_MRA | NF_ADD_LINK);
        $listener = &atknew("modules.crm.listeners.eventlistener");
        $this->addListener($listener);
        $this->add(new atkAttribute("id", AF_AUTOKEY));
        $this->add(new atkAttribute("title", AF_OBLIGATORY | AF_SEARCHABLE));
        $this->add(new atkManyToOneRelation("type", "crm.campaign_type", AF_SEARCHABLE));
        $this->add(new atkManyToOneRelation("status", "crm.campaign_status", AF_SEARCHABLE));
        $this->add(new atkDateAttribute("start_date"));
        $this->add(new atkDateAttribute("end_date"));
        $this->add(new atkCurrencyAttribute("budget", AF_HIDE_LIST));
        $this->add(new atkCurrencyAttribute("actual_cost", AF_HIDE_LIST));
        $this->add(new atkCurrencyAttribute("expected_revenue", AF_HIDE_LIST));
        $this->add(new atkCurrencyAttribute("expected_cost", AF_HIDE_LIST));
        $this->add(new atkTextAttribute("objective", AF_HIDE_LIST));
        $this->add(new atkTextAttribute("description", AF_HIDE_LIST));

        $this->add(new atkCreatedByAttribute("created_by", AF_HIDE_LIST), "history");
        $this->add(new atkCreateStampAttribute("created_on", AF_HIDE_LIST), "history");
        $this->add(new atkUpdatedByAttribute("last_modified_by", AF_HIDE_LIST), "history");
        $this->add(new atkUpdateStampAttribute("last_modified_on", AF_HIDE_LIST), "history");


        $this->setOrder("title");
        $this->setTable("crm_campaign");
        $this->setDescriptorTemplate('[title]');
    }

    /**
     * Delete campaign from event history when it's deleted
     *
     * @param array $record Campaign record
     * @return boolean
     */
    function postDelete($record)
    {
        $eventlog = &atkGetNode("crm.eventlog");
        $eventlog->delete_event_history($this->atknodetype(), $record["atkprimkey"]);
        return true;
    }

}

?>