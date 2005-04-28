<?php

  userelation("atkmanytoonerelation");
  useattrib("atknumberattribute");
  useattrib("atkdateattribute");
  useattrib("timereg.manualattribute");
  
  class overtime_balance extends atkNode 
  {
    function overtime_balance()
    {
      $this->atkNode("overtime_balance");
      $this->add(new atkAttribute("id",AF_AUTOKEY));
      $this->add(new atkManyToOneRelation("userid", "employee.employee", AF_OBLIGATORY|AF_SEARCHABLE|AF_READONLY));
      $this->add(new atkDateAttribute("day","F j Y","F j Y", 0, date("Y-m-d"), AF_SEARCHABLE|AF_OBLIGATORY));
      $this->add(new atkNumberAttribute("balance", AF_OBLIGATORY, 10,2)); 
      $this->add(new manualAttribute("manual", AF_HIDE, 1, 0));
      
      $this->setTable("overtime_balance", "overtime_balance");
      $this->m_securityMap["add"] = "edit";
      $this->m_securityMap["save"] = "edit";
    }
    
    function postUpdate($rec)
    {
      $this->deleteBalance($rec["userid"]["id"], date("Y-m-d", adodb_mktime(0,0,0,$rec["day"]["month"],$rec["day"]["day"],$rec["day"]["year"])));
    }

    function postAdd($rec)
    {
      $this->deleteBalance($rec["userid"]["id"], date("Y-m-d", adodb_mktime(0,0,0,$rec["day"]["month"],$rec["day"]["day"],$rec["day"]["year"])));
    }
    
    function postDel($rec)
    {
      $this->deleteBalance($rec["userid"]["id"], date("Y-m-d", adodb_mktime(0,0,0,$rec["day"]["month"],$rec["day"]["day"],$rec["day"]["year"])));
    }

    function getOvertimeLink($rec)
    {      
      if ($rec["balance"]["balance"] != text("not_inserted")) $res = time_format(60*$rec["balance"]["balance"], true);
      else $res = $rec["balance"]["balance"];

      $manual = $this->getLatestManualCorrection($rec["balance"]["userid"]["id"], 0, date("Y-m-d", adodb_mktime(0,0,0,$rec["balance"]["day"]["month"],$rec["balance"]["day"]["day"],$rec["balance"]["day"]["year"])));
      $manualid = $manual["id"];
      if ($manual != 0)
      {
        // check if this manualcorrection was made in the current week
        $enddate = date("Y-m-d", adodb_mktime(0,0,0,$rec["balance"]["day"]["month"],$rec["balance"]["day"]["day"],$rec["balance"]["day"]["year"]));
        $startdate = startOfWeek($enddate);
        $manualdate = date("Y-m-d", adodb_mktime(0,0,0,$manual["day"]["month"],$manual["day"]["day"],$manual["day"]["year"]));
        if ($manualdate <= $enddate && $manualdate >= $startdate) $manual = 1;
        else $manual = 0;
      }
      
      if ($this->allowed("edit") && $rec["balance"]["outputtype"] == 0)
      {        
        if ($rec["balance"]["id"] != "" && $manual)
        {
          $url = dispatch_url("timereg.overtime_balance", "edit", array("atkselector"=>"overtime_balance.id = ".$manualid));
        }
        elseif ($rec["balance"]["id"] != "")
        {
          $url = dispatch_url("timereg.overtime_balance", "edit", array("atkselector"=>"overtime_balance.id = ".$rec["balance"]["id"]));
        }
        else
        {
          $url = dispatch_url("timereg.overtime_balance", "add", array("userid[id]"=>$rec["balance"]["userid"]["id"], "day[day]"=>$rec["balance"]["day"]["day"], "day[month]"=>$rec["balance"]["day"]["month"], "day[year]"=>$rec["balance"]["day"]["year"], "balance"=>"0.00"));
        }
        $res = href($url, $res, SESSION_NESTED);
      }
      
      if ($this->allowed("delete") && $manual)
      {
        $deletelink = href(dispatch_url("timereg.overtime_balance", "delete", array("atkselector"=>"overtime_balance.id = ".$manualid)), text("manualovertime_delete"), SESSION_NESTED);
      }
      else $deletelink = "";
      return $res.($manual ? ' *' : '').' '.$deletelink;
    }
    
    function getBalance($day, $userid)
    {
      $node = &getNode("timereg.overtime_balance");
      $result = $node->selectDb("day <= '$day' AND userid.id = '$userid'", "day DESC", 1);
      if (!count($result)) return $this->getDummyBalance($day, $userid);
      else
      {
        $endtime = adodb_mktime(0,0,0,$result[0]["day"]["month"],$result[0]["day"]["day"],$result[0]["day"]["year"]);
        $starttime = adodb_mktime(0,0,0,substr($day,5,2),substr($day,8,2),substr($day,0,4));
        if ($endtime < $starttime)
        {
          //recalculate
          return $this->recalculateBalance(date("Y-m-d", adodb_mktime(0,0,0,$result[0]["day"]["month"],$result[0]["day"]["day"]+1,$result[0]["day"]["year"])), $day, $userid, $result[0]["balance"]);
        }
        else if (count($result)) return $result[0];
      }
    }
    
    function getDummyBalance($day, $userid, $balance="", $id=0)
    {
      if ($balance === "") $res["balance"] = text("not_inserted"); else $res["balance"] = $balance;
      $res["userid"]["id"] = $userid;
      $res["day"]["day"] = substr($day,8,2);
      $res["day"]["month"] = substr($day,5,2);
      $res["day"]["year"] = substr($day,0,4);
      if ($id != 0) $res["id"] = $id;
      $res["manual"] = 0;
      return $res;
    }
    
    function recalculateBalance($begin, $end, $userid, $oldbalance)
    {
      // calculate new balance
      $newbalance = $this->calculateNewBalance($userid, $begin, $end, $oldbalance);
      // update database with new balance
      $id = $this->updateOvertimeBalance($userid, $end, $newbalance);
      return $this->getDummyBalance($end, $userid, $newbalance, $id);
    }
    
    function getLatestManualCorrection($userid, $default, $day="")
    {
      //global $g_db;
      //$result = $g_db->getRows("SELECT day FROM overtime_balance WHERE userid = $userid AND manual = 1 ORDER BY day DESC");
      $node = &getNode("timereg.overtime_balance");
      $result = $node->selectDb("userid.id = '$userid' AND manual = 1 ".($day != "" ? "AND day <= '$day' " : ""), "day DESC", 1);
      return (count($result[0]) ? $result[0] : $default);
    }
    
    function deleteBalance($userid, $day)
    {
      global $g_db;
      $g_db->query("DELETE FROM overtime_balance WHERE manual <> 1 AND day > '$day'");
    }
    
    function addOvertimeBalance($userid, $day, $balance)
    {
      global $g_db;
      $newid = $g_db->nextid("overtime_balance");
      $g_db->query("INSERT INTO overtime_balance (id, userid, day, balance, manual)
                  VALUES ('$newid', '$userid', '$day', '$balance', 0)");
    }
    
    function saveOvertimeBalance($userid, $day, $balance)
    {
      global $g_db;
      $sql = "UPDATE overtime_balance SET balance = $balance WHERE userid = ".$userid." AND day = '$day'";
      $g_db->query($sql);
    }
    
    function getPercentage($id)
    {
      global $g_db;
      if ($id!="")
      {
        $result = $g_db->getRows("SELECT percentage FROM workperiod WHERE id = ".$id);
      }
      return (count($result) ? $result[0]["percentage"] : 100);
    }
    
    function updateOvertimeBalance($userid, $day, $balance)
    {
      global $g_db;
      $recs = $g_db->getrows("SELECT id FROM overtime_balance WHERE userid = $userid AND day = '$day'");
      if (!count($recs))
      {
        $newid = $g_db->nextid("overtime_balance");
        $g_db->query("INSERT INTO overtime_balance (id, userid, day, balance, manual)
                  VALUES ('$newid', '$userid', '$day', '$balance', 0)");
        return $newid;
      }
      else
      {
        $g_db->query("UPDATE overtime_balance SET balance = $balance WHERE userid = $userid AND day = '$day'");
        return $recs[0]["id"];
      }
    }
    
    function calculateNewBalance($userid, $begin, $end, $oldbalance)
    {
      $percentages = $this->getPercentages();
      $contracthours = $this->getContractHours($begin, $end, $userid);
      
      $hours = $this->getUserHours($begin, $end, $userid);
      $total = 0;
      for ($i=0;$i<count($hours);$i++)
      {
        $time = $hours[$i]["time"]/60;
        if (count($percentages) && $hours[$i]["workperiod"] != 0) $percentage = $percentages[$hours[$i]["workperiod"]];
        else $percentage = 1;
        $total += $percentage * $time;
      }
      return ($oldbalance + ($total - $contracthours));
    }
    
    function getUserHours($begin, $end, $userid)
    {
      global $g_db;
      $sql = "SELECT * FROM hours WHERE userid = '$userid' AND activitydate >= '$begin' AND activitydate <= '$end'";
      return $g_db->getRows($sql);
    }
    
    function getPercentages()
    {
      global $g_db;
      $res = $g_db->getRows("SELECT id, percentage FROM workperiod");
      $result = array();
      for ($i=0;$i<count($res);$i++)
      {
        $result[$res[$i]["id"]] = $res[$i]["percentage"]/100;
      }
      return $result;
    }
    
    function getContractHours($begin, $end, $userid)
    {
      global $g_db;
      
      $begindate = date("Ymd", adodb_mktime(0,0,0,substr($begin,5,2),substr($begin,8,2),substr($begin,0,4)));
      $enddate = date("Ymd", adodb_mktime(0,0,0,substr($end,5,2),substr($end,8,2),substr($end,0,4)));
      
      //$sql = "SELECT uc_hours, startdate, enddate, WorkingDays FROM usercontract WHERE userid = '$userid' AND ((startdate <= '$begin' AND enddate >= '$begin') OR (startdate <= '$end' AND enddate >= '$end'))";
      $sql = "SELECT uc_hours, startdate, enddate, WorkingDays FROM usercontract WHERE userid = '$userid' AND startdate <= '$end' AND (enddate >= '$begin' OR enddate IS NULL)";
      $contracts = $g_db->getRows($sql);
      
      if (count($contracts) > 1)
      {
        $total = 0;
        for ($i=0;$i<count($contracts);$i++)
        {
          $workdays = explode("|",$contracts[$i]["WorkingDays"]);
          $startdate = date("Ymd", adodb_mktime(0,0,0,substr($contracts[$i]["startdate"],5,2),substr($contracts[$i]["startdate"],8,2),substr($contracts[$i]["startdate"],0,4)));
          $newenddate = date("Ymd", adodb_mktime(0,0,0,substr($contracts[$i]["enddate"],5,2),substr($contracts[$i]["enddate"],8,2),substr($contracts[$i]["enddate"],0,4)));
          if ($startdate <= $begindate && $newenddate >= $enddate)
          {
            $numdays = $this->getNumDays($begindate, $enddate, $workdays);
            $total += ($numdays/count($workdays)) * $contracts[$i]["uc_hours"];
          }
          elseif ($startdate <= $begindate)
          {
            // contract ends this week
            $numdays = $this->getNumDays($begindate, $newenddate, $workdays);
            $total += ($numdays/count($workdays)) * $contracts[$i]["uc_hours"];
          }
          else
          {
            // contract starts this week
            $numdays = $this->getNumDays($startdate, $enddate, $workdays);
            $total += ($numdays/count($workdays)) * $contracts[$i]["uc_hours"];
          }
        }
        return $total;
      }
      else
      {
        $workdays = explode("|",$contracts[0]["WorkingDays"]);
        $numdays = $this->getNumDays($begindate, $enddate, $workdays);
        return (count($contracts)>0 ? (($numdays/count($workdays)) * $contracts[0]["uc_hours"]) : 0);
      }
    }
    
    function getNumDays($begin, $end, $workdays)
    {
      $total = 0;
      $starttime = adodb_mktime(0,0,0,substr($begin,4,2),substr($begin,6,2),substr($begin,0,4));
      $endtime = adodb_mktime(0,0,0,substr($end,4,2),substr($end,6,2),substr($end,0,4));
      
      for($i=0;$i<=(($endtime-$starttime)/(60*60*24));$i++)
      {
        $dayofweek = date("w", adodb_mktime(0,0,0,substr($begin,4,2),substr($begin,6,2)+$i,substr($begin,0,4)));
        if (in_array($dayofweek, $workdays)) $total++;
      }
      return $total;
    }
  }

?>
