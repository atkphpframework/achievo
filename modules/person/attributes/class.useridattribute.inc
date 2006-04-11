<?php

atkimport("atk.attributes.atkattribute");
class useridAttribute extends atkAttribute
{
  function useridAttribute($name, $flags=0)
  {
    /* base class constructor */
    $this->atkAttribute($name, $flags, 15);
  }

  function value2db($rec)
  {
    return escapeSQL(str_replace(" ", "_", strtolower($rec["userid"])));
  }
}


?>