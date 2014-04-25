<?php
useattrib('atknumberattribute');
useattrib('atktextattribute');
useattrib('atkmultiselectattribute');
useattrib('atktimeattribute');

class tpl_contract extends atkNode
{

    public function __construct()
    {
        $m_arr_minute_step = array();
        for ($i = 0; $i < 60; $i+=5)
            $m_arr_minute_step[] = sprintf("%02d", $i);

        $this->atkNode('tpl_contract', NF_ADD_LINK);
        $this->add(new atkNumberAttribute("id", AF_AUTOKEY));
        $this->add(new atkAttribute('name', AF_OBLIGATORY));
        $this->add(new atkNumberAttribute("uc_hours", AF_OBLIGATORY, 4, 2));
        $this->add(new atkTextAttribute("description", TEXT_SMALL, 0));
        $this->add(new atkMultiSelectAttribute("workingdays", array("mon", "tue", "wed", "thu", "fri", "sat", "sun"), array(1, 2, 3, 4, 5, 6, 0), 7, AF_HIDE_LIST));
        $this->add(new atkTimeAttribute("workstarttime", 0, 23, $m_arr_minute_step, "08:30", AF_OBLIGATORY));
        $this->add(new atkTimeAttribute("workendtime", 0, 23, $m_arr_minute_step, "17:00", AF_OBLIGATORY));
        $this->setTable("tpl_usercontract");
        $this->setDescriptorTemplate('[name]');
    }

    /**
     * Set initial values for a contract
     *
     * @return array
     */
    function initial_values()
    {
        $nextyear = strtotime("+1 year");
        return array("uc_hours" => atkConfig::get('employee', 'default_weekly_contract_hours', 40),
            "startdate" => array("year" => (date("Y")),
                "month" => date("m"),
                "day" => date("d")),
            "enddate" => array("year" => date("Y", $nextyear),
                "month" => date("m", $nextyear),
                "day" => date("d", $nextyear)),
            "workstarttime" => array("hours" => "08",
                "minutes" => "30",
                "seconds" => "00"),
            "workendtime" => array("hours" => "17",
                "minutes" => "00",
                "seconds" => "00"),
        );
    }

}
