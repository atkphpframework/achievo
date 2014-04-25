<?php

class test_overtime_balance extends atkTestCase
{

    function test_numdays_usingworkdays()
    {
        $ob = &atkGetNode("timereg.overtime_balance");
        $this->assertEqual($ob->getNumDays("20070530", "20070530", array(3)), 1);
        $this->assertEqual($ob->getNumDays("20070530", "20070530", array(4)), 0);
        $this->assertEqual($ob->getNumDays("20070501", "20070531", array(2, 4)), 10);
        $this->assertEqual($ob->getNumDays("20070501", "20070531", array()), 31);
        $this->assertEqual($ob->getNumDays("20070530", "20070530", array()), 1);
        $this->assertEqual($ob->getNumDays("20070101", "20071231", array()), 365);
        $this->assertEqual($ob->getNumDays("20070527", "20070527", array(1, 2, 3, 4, 5)), 0);
        $this->assertEqual($ob->getNumDays("20070528", "20070528", array(1, 2, 3, 4, 5)), 1);
        $this->assertEqual($ob->getNumDays('20070102', '20070527', array(1, 2, 3, 4, 5)), 104);
        $this->assertEqual($ob->getNumDays('20070102', '20070528', array(1, 2, 3, 4, 5)), 105);
        $this->assertEqual($ob->getNumDays('20070301', '20070325', array()), 25);
        $this->assertEqual($ob->getNumDays('20070301', '20070326', array()), 26);
        $this->assertEqual($ob->getNumDays('20071001', '20071027', array()), 27);
        $this->assertEqual($ob->getNumDays('20071001', '20071028', array()), 28);
    }

}

?>