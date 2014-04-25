<?php

class organizationselector extends atkNode
{

    function organizationselector()
    {
        $this->atkNode("organizationselector", NF_EDITAFTERADD | NF_EXPORT);

        $this->add(new atkNumberAttribute("id", AF_AUTOKEY));
        $this->add(new atkAttribute("organizationcode", AF_SEARCHABLE));
        $this->add(new atkAttribute("name", AF_SEARCHABLE | AF_OBLIGATORY, 100));
        $this->add(new atkAttribute("visit_city", AF_SEARCHABLE, 100));
        $this->add(new atkListAttribute("type", array("customer", "supplier"), "", AF_HIDE_LIST));
        $this->setTable("organization", "organization");
        $this->setOrder("organization.name");
        $this->setSecurityAlias('organization.organization');
        $this->setDescriptorTemplate('[name] [visit_city] ([organizationcode])');
    }

    function descriptor($rec = "")
    {
        if (!empty($rec)) {
            $tpl = '[name]';
            if ($rec['visit_city'] != '')
                $tpl.=', [visit_city]';
            if ($rec['organizationcode'] != '')
                $tpl.=' ([organizationcode])';
            $this->setDescriptorTemplate($tpl);
        }
        return parent::descriptor($rec);
    }

}

?>