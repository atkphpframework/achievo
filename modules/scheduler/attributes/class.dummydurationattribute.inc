<?php



// Same as listattribute, but then without save

class dummydurationattribute extends atkListAttribute
{

  function dummydurationattribute($name)
  {
    $flags = AF_HIDE_SEARCH | AF_NO_SORT | AF_LIST_NO_NULL_ITEM | AF_NO_TRANSLATION;
    $optionArray = array('0:15','0:30','0:45','1:00','1:30','2:00','2:30','3:00','4:00','5:00','6:00','7:00','8:00',atkText('use_enddate', 'scheduler'));
    $valueArray = array(15,30,45,60,90,120,150,180,240,300,360,420,480,-1);
    parent::atkListAttribute($name,$optionArray,$valueArray,$flags,$size);
  }

  function search($record="", $size=0, $maxsize=0)
  {
    return "&nbsp;";
  }

 /**
  * No function, but is neccesary
  */
  function store($a, $b)
  {
    return true;
  }

  function db2value($record)
  {
    atk_var_dump($record,"DURATION RECORD");
    $tmp_startdate = explode('-',$record['startdate']);
    $tmp_enddate   = explode('-',$record['enddate']);
    $tmp_starttime = explode(':',$record['starttime']);
    $tmp_endtime   = explode(':',$record['endtime']);


    $startdate = mktime(intval($tmp_starttime[0]),intval($tmp_starttime[1]),intval($tmp_starttime[2]),intval($tmp_startdate[1]),intval($tmp_startdate[2]),intval($tmp_startdate[0]));
    $enddate   = mktime(intval($tmp_endtime[0]),intval($tmp_endtime[1]),intval($tmp_endtime[2]),intval($tmp_enddate[1]),intval($tmp_enddate[2]),intval($tmp_enddate[0]));
    $diff = ($enddate-$startdate)/60;
    $valueArray = array(15,30,45,60,90,120,150,180,240,300,360,420,480);
    atkdebug("DURATION: DifF: $diff");
    if(in_array($diff,$valueArray)) return $diff;
    return -1;
  }

  /**
  * No function, but is neccesary
  */
  function addToQuery(&$query, $tablename="", $fieldaliasprefix="",$level)
  {
  }

  function getSearchModes()
  {
    // exact match and substring search should be supported by any database.
    // (the LIKE function is ANSI standard SQL, and both substring and wildcard
    // searches can be implemented using LIKE)
    // Possible values
    //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
    return array();
  }

  function dbFieldType()
  {
    return "";
  }

  function needsUpdate()
  {
    return false;
  }


}


?>