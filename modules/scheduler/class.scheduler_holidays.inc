<?php

/**
 * Module Calendar
 * Class schedule_attendee
 *
 * This class makes it possible to add attendees to a schedule. These attendees can be employees and contacts.
 * That is why the relation is linked to person.
 *
 * $Id$
 * @author Sandy Pleyte <sandy@achievo.org>
 * @version $Revision$
 *
 */

useattrib("atkboolattribute");
useattrib("atklistattribute");
useattrib("atknumberattribute");
userelation("atkmanytoonerelation");

include moduleDir("scheduler")."utils/class.calculateHoliday.inc";
include moduleDir("scheduler")."utils/class.xmlfunctions.inc";

class scheduler_holidays extends atkNode
{
  var $m_country_filter = "";

  function scheduler_holidays()
  {
    global $g_sessionManager;

    $this->m_country_filter = $g_sessionManager->pageVar("country_filter");

    $this->atkNode("scheduler_holidays",NF_MRA|NF_ADD_LINK);

    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("name",AF_OBLIGATORY|AF_SEARCHABLE));
    $this->add(new atkListAttribute("type",array("weekend","info"),array("weekend","info"),AF_SEARCHABLE|AF_OBLIGATORY));
    $this->add(new atkListAttribute("day",array(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31),array(),AF_HIDE_LIST));
    $this->add(new atkListAttribute("month",array(1,2,3,4,5,6,7,8,9,10,11,12),array(),AF_HIDE_LIST));
    $this->add(new atkListAttribute("special",array("easter"),array("easter"),AF_HIDE_LIST));
    $this->add(new atkNumberAttribute("offset",AF_HIDE_LIST,3,0));
    $this->add(new atkListAttribute("weekday",array("monday","tuesday","wednesday","thursday","friday","saturday","sunday"),array(0,1,2,3,4,5,6),AF_HIDE_LIST));
    $this->add(new atkListAttribute("moveto",array("ifsatorsunthen","ifsunthen","ifsatorsunthen"),array("1","2","3"),AF_HIDE_LIST));
    $this->add(new atkBoolAttribute("flag",AF_HIDE_LIST));
    $this->add(new atkManyToOneRelation("schedulecategory", "scheduler.scheduler_category",AF_OBLIGATORY|AF_RELATION_AUTOLINK));
    $this->add(new atkAttribute("country"));

    if($this->m_country_filter!="" && $this->m_country_filter!="all")
    {
      $this->addFilter("country='".$this->m_country_filter."'");
    }
    $this->setTable("scheduler_holidays");
  }

  function postUpdate($rec,$mode="add")
  {
    $this->_clearCache();
    return true;
  }

  function postAdd($rec)
  {
    $this->_clearCache();
    return true;
  }

  function postDel($rec)
  {
    $this->_clearCache();
    return true;
  }

  function action_import()
  {
     //$xml = new xmlfunctions();
     //$holidays = $xml->importXML();
  }

  function action_export()
  {
     $xml = &atknew("modules.scheduler.utils.xmlfunctions");
     $holidays = $this->selectDb();

     $xmlfile = $xml->exportXML($holidays);
     $exporter = &atknew("atk.utils.atkfileexport");
     $exporter->export($xmlfile, "holidays","xml","xml");
  }

  function adminHeader()
  {
    $g_db = &atkGetDb();
    $ret = '<form name="country_select" method="get" action="'.$_SERVER["PHP_SELF"].'" onChange="document.forms[country_select].submit();">'."\n";
    $ret.= session_form();
    $ret.= $this->text("select_country").':&nbsp;';
    $ret.= '<select name="country_filter">';
    $ret.= '<option value="all">'.$this->text("all_countries").'</option>';
    $sql = "SELECT DISTINCT country FROM scheduler_holidays";
    $nrows = $g_db->getrows($sql);
    for($i=0,$_i=count($nrows);$i<$_i;$i++)
    {
      if($nrows[$i]["country"]==$this->m_country_filter) { $sel = " SELECTED"; } else { $sel=""; }
      $ret.='<option value="'.$nrows[$i]["country"].'"'.$sel.'>'.$nrows[$i]["country"].'</option>\n';
    }
    $ret.= '</select>&nbsp;<input type="submit" value="'.atkText("select").'">&nbsp;&nbsp;';
    $ret.= href(dispatch_url("scheduler.scheduler_holidays", "import", array()),atkText("import"));
    $ret.= "&nbsp;&nbsp";
    $ret.= href(dispatch_url("scheduler.scheduler_holidays", "export", array()),atkText("export"));
    $ret.= '</form>'."\n";
    return $ret;
  }

 /**
  * Check if a date is a holiday
  *
  * @param string $date Date in yyyymmdd or yyyy-mm-dd format
  * @param string $type Check for weekend/info
  * @return bool Is holiday ?
  */
  function isHoliday($date,$type="")
  {
    $date = str_replace("-","",$date);
    $year = substr($date, 0,4);
    $holidays = $this->_getHolidays($year);
    if($type=="")
    {
      return ($holidays[$date]["name"]!=""?true:false);
    }
    else
    {
      return ($holidays[$date]["name"]!=""&&$holidays[$date]["type"]==$type?true:false);
    }
  }

 /**
  * Get the holiday info for a date
  *
  * @param string $date Date in yyyymmdd or yyyy-mm-dd format
  * @return array Array with name, type, schedulertype
  */
  function getHolidayInfo($date)
  {
    $date = str_replace("-","",$date);
    $year = substr($date, 0,4);
    $holidays = $this->_getHolidays($year);

    return $holidays[$date];
  }

 /**
  * Get holidays for a year from the cache, file, database
  *
  * @param int $year year
  * @return array Array with the holidays
  */
  function _getHolidays($year)
  {
    static $holidays="";
    $filename = $this->_getFilename($year);
    if(is_array($holidays))
    {
      return $holidays;
    }
    elseif(file_exists($filename))
    {
      include $filename;
    }
    else
    {
      $db_holidays = $this->selectDb();
      $holidays = array();
      $calculator = new calculateHoliday();
      foreach($db_holidays as $id => $holiday)
      {
        $date = date("Ymd",$calculator->getHolidayDate($holiday,$year));
        $holidays[$date] = array("name"=>$holiday["name"],
                                 "type"=>$holiday["type"],
                                 "scheduletype"=>$holiday["scheduletype"]["id"]);
      }
      $this->_saveHolidays($holidays,$year);
    }
    return $holidays;
  }

 /**
  * Generate a filename for the holidays
  * @param int $year year
  * @return string Filename
  */
  function _getFilename($year)
  {
    return atkconfig("atktempdir")."holidays/".$year.".inc";
  }

 /**
  * Save the calculated holidays to a file
  *
  * @param array $holidays Array with the holidays
  * @param int $year Year
  */
  function _saveHolidays($holidays,$year)
  {
    return true;
    $filename = $this->_getFilename($year);
    $fp = fopen($filename,"w");
    $content="<?php\n
              \t\$holidays = array(\n";
    foreach($holidays as $date => $holiday)
    {
      $content.="'$date'=>array('name'=>'".$holiday["name"]."',
                               'type'=>'".$holiday["type"]."',
                               'scheduletype'=>'".$holiday["scheduletype"]."'),\n";
    }
    $content.=");\n
               ?>";
    if (fwrite($fp, $content) === FALSE) {
       echo "Cannot write to file ($filename)";
       exit;
   }

    fclose($fp);
  }

 /**
  * Remove all generated holiday files
  */
  function _clearCache()
  {
    $dir = atkconfig("atktempdir")."holidays";
    if ($handle = opendir($dir)) {
      while (false !== ($file = readdir($handle))) {
        if ($file != "." && $file != "..") {
           unlink($dir."/".$file);
        }
      }
      closedir($handle);
    }
  }

}
?>