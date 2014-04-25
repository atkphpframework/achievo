<?php

class manualAttribute extends atkAttribute
{

    function manualAttribute($name, $flags = 0)
    {
        $this->atkAttribute($name, $flags);
    }

    function needsUpdate($record)
    {
        return true;
    }

    function value2db($rec)
    {
        return 1;
    }

}

?>