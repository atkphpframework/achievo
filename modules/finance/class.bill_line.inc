<?php

function getbilledamount($billedamount)
{
   global $g_db;
   
   
}

class bill_line extends atkNode
{
  function bill_line()
  {
    $fixed_price = sessionLoad("fixed_price");  
     
    $this->atkNode("bill_line", NF_NO_EDIT);

    $this->addAttribute(new atkAttribute("id" ,AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkAttribute("shortdescription",AF_OBLIGATORY));
    $this->addAttribute(new atkTextAttribute("description"));
    
    if ($fixed_price > 0)
    {
      $this->addAttribute(new atkDummyAttribute("fixed","",AF_HIDE_LIST));
      $this->add(new atkRadioAttribute("calcoption",array(text("fixedcalc"),text("aftercalc"),text("discount"),text("costs")),array("fixed","calc","discount","costs"), AF_OBLIGATORY));
      $this->addAttribute(new atkDummyAttribute("fixedamount","",AF_HIDE_LIST));
      $this->add(new atkAttribute("fixed_billed"));
    }
    else
    {
      $this->add(new atkRadioAttribute("calcoption",array(text("aftercalc"),text("discount"),text("costs")),array("calc","discount","costs"), AF_OBLIGATORY));
    }
    $this->addAttribute(new atkManyToOneRelation("billid","finance.bill",AF_OBLIGATORY|AF_HIDE_LIST|AF_HIDE));
    
    $this->setTable("bill_line","bill_line");
    $this->setOrder("id");
    
    atkdebug("bill_line::bill_line()");
    $this->m_securityMap["addtobill"] = "add";
    $this->m_securityMap["nohours"] = "add";
    $this->m_securityMap["addcosts"] = "add";
    $this->m_securityMap["adddiscount"] = "add";
  }
  
  function initial_values()
  {
    global $g_user;
    $initial_date = Array("year"=>date("Y"), "month"=>date("m"), "day"=>date("d"));
    return array("create_date"=>$initial_date,"userid"=>strtolower($g_user["name"]));
  }
 
 function fixed_edit(&$rec)
 {
   global $g_db, $g_layout;
   
   $fixed_price = sessionLoad("fixed_price");
   
   $sql = "SELECT projectid FROM bill WHERE id = " . $rec[billid][id];
   $projectid=$g_db->getrows($sql);
   
   $sql = "SELECT id FROM bill WHERE projectid = " . $projectid["0"]["projectid"];
   $bill_line_rec=$g_db->getrows($sql);
   
   for ($i=0;$i<count($bill_line_rec);$i++)
   {
     $bill_line_rec2[] = $bill_line_rec[$i]["id"];
   }
   
   $sql = "SELECT fixed_billed FROM bill_line WHERE calcoption = 'fixed' AND billid IN (".implode(",",$bill_line_rec2).") ";
   $fixedrec=$g_db->getrows($sql);
   
   $sql = "SELECT fixed_billed FROM bill_line WHERE calcoption = 'fixed' AND billid = ".$rec[billid][id];
   $this_fixedrec=$g_db->getrows($sql);
      
   if(count($fixedrec) <= 1)
   {
     if(count($fixedrec) == 1)
     {    
       $billedamount = $fixedrec[0]["fixed_billed"];
     }  
     else
     {
       $billedamount = 0;
     }
   }   
   else
   {
    for($i=0;$i<count($fixedrec);$i++)
     {   
       $billedamount += $fixedrec[$i]["fixed_billed"];
     }
   }
   
   if(count($this_fixedrec) <= 1)
   {
     if(count($this_fixedrec) == 1)
     {    
       $this_billedamount = $this_fixedrec[0]["fixed_billed"];
     }  
     else
     {
       $this_billedamount = 0;
     }
   }   
   else
   {
    for($i=0;$i<count($this_fixedrec);$i++)
     {   
       $this_billedamount += $this_fixedrec[$i]["fixed_billed"];
     }
   }
   
   $g_layout->output('<input type="hidden" name="billedamount" value="' .$billedamount.'">');
   $text = "<B>" . text("warning_fixedprice1") . round($fixed_price,2) . "! " . $billedamount . " " . text("warning_fixedprice3") ." ". $this_billedamount." ".text("warning_fixedprice4")."</B>";
   return $text;
 }
 
 function fixedamount_edit($rec)
 {
   $text = "<B>" . text("fixedamount") ."</B>";
   return $text;
 }
 
 function fixed_billed_validate($rec)
 {
    global $billedamount;
    
    $fixed_price = sessionLoad("fixed_price");
    
    if ($rec["fixed_billed"] > ($fixed_price - $billedamount))
    {
      triggerError(&$rec, $this->m_attribList["fixed_billed"], 'error_billamount_tohigh');
      return false;
    }
 }
 
 function action_save()
 {
      global $PHP_SELF;
      if ($this->m_postvars['atkcancel']=="")
      {
        $record = $this->updateRecord();
        $this->validate($record, "add");
        if (count($record['atkerror'])>0)
        {
          $this->addPage($record);
        }
        else
	{   
          $this->addDb($record);
	}
      }  
      else
      {   
        // Cancel was pressed
        $this->redirect();   
      }
  }
  
  function action_createbill()
  {
    global $bill_line_id, $calcoption;
    include "./modules/finance/createbill.php";
  }
  
  function action_adddiscount()
  {
    //this function adds discounts
    global $costcount, $g_db, $bill_line_id, $dis_option, $amount, $apply_on;
    
    $sql = " INSERT INTO discount (type, amount, bill_line_id, apply_on) VALUES ('$dis_option', '$amount', '$bill_line_id', '$apply_on')";
    $rec=$g_db->getrows($sql);
    
    $this->redirect();
    
  }
  
  function action_addcosts()
  {
    //this function adds the costs and the bill_line_id into the right costregistration records.
    global $costcount, $g_db, $bill_line_id;
    
    for ($i=1;$i<($costcount);$i++)
    {
      $reg = "checkA".$i;
      $amount = "costamount".$i;
      $value1 = $GLOBALS[$reg];
      $value2 = $GLOBALS[$amount];
      if ($value1 != "") 
      {     
        $regArr[] = $value1;
	$costArr[] = $value2;
      }
    }
    //Check if there are any hours selected. 
    if ($regArr == NULL)
    {
      echo "You didn't select any costs, please go back and select some costs!";
      exit;
    }
    
    //add the calculated amount of the costs (default currency) into the right records.
    for ($i=0;$i<count($costArr);$i++)
    {
      $sql = " UPDATE costregistration SET costamount = ".$costArr[$i]. " WHERE id = ".$regArr[$i];
      $rec=$g_db->getrows($sql);
    }
    
    //check which hours are selected to do a proper Query  
    if ($regArr > NULL)
    {
      $sql = " UPDATE costregistration SET bill_line_id = ".$bill_line_id. " WHERE id IN (".implode(",",$regArr).") ";
      $rec=$g_db->getrows($sql);
    }
    $this->redirect();
  }
  
  function action_addtobill()
  {
   //this function adds the billid into the right hours records that were selected.
   global $hourcount, $bill_id, $g_db, $bill_line_id, $g_layout, $calcoption;
   
   for ($i=1;$i<($hourcount);$i++)
   {
     $reg = "checkA".$i;
     $value1 = $GLOBALS[$reg];
     if ($value1 != "") 
     {     
       $regArr[] = $value1;
     }
     
     $hourid = "checkB".$i;
     $rate = "rate".$i;   
     $value2 = $GLOBALS[$hourid];
     $value3 = $GLOBALS[$rate];
        
     if ($value2 != "") 
     {     
       $houridArr[] = $value2;
       $rateArr[] = $value3;
     }
               
   } 
   
   //Check if there are any hours selected. 
   if ($regArr == NULL)
   {
     $g_layout->ui_top(text("title_nohours"));
     $g_layout->output(text("bill_nohours"));
     $g_layout->output('<form action="dispatch.php" method="post" name="generatebillform">');
     $g_layout->output(session_form());
     $g_layout->output('<input type="hidden" name="atknodetype" value="finance.bill_line">');
     $g_layout->output('<input type="hidden" name="atkaction" value="nohours">');
     $g_layout->output('<input type="hidden" name="bill_id" value='.$bill_id.'>');
     $g_layout->output('<input type="hidden" name="bill_line_id" value="'.$bill_line_id.'">');
     $g_layout->output('<input type="hidden" name="calcoption" value="'.$calcoption.'">');
     $g_layout->table_simple();
     $g_layout->output('<tr>');
     $g_layout->td('<BR><BR><input type="submit" name="knob" value="'.text("bill_knob_goback").'"><input type="submit" name="knob" value="'."Cancel".'">');
     $g_layout->output('</tr>');
     $g_layout->output("</FORM>");
     $g_layout->output('</table>');     
   }
   else
   {
	   $rates = array();
	   for ($i=0;$i<count($houridArr);$i++)
	   {
	     $temp = $rateArr[$i];
	     $rates["$temp"][] = $houridArr[$i];
	   }
	   
	   foreach ($rates as $rate => $idArr)
	   {
	     $sql = " UPDATE hours SET hour_rate = ".$rate. " WHERE id IN (".implode(",",$idArr).") ";
	     $xrec=$g_db->getrows($sql);   
	   }
	      
	   //check which hours are selected to do a proper Query  
	   if ($houridArr > NULL)
	   {
	     $sql1 = " UPDATE hours SET bill_line_id = ".$bill_line_id. ", onbill = 1 WHERE id IN (".implode(",",$houridArr).") ";
	     $rec=$g_db->getrows($sql1);
	   }
	   
	   if ($regArr > NULL)
	   {
	     $sql2 = " UPDATE hours SET bill_line_id = ".$bill_line_id. ", registered = 1 WHERE id IN (".implode(",",$regArr).") ";
	     $rec=$g_db->getrows($sql2);
	   }
	
	   $this->redirect();
    }
               
  }
  
  function action_nohours()
  {
    global $knob, $bill_line_id, $g_db, $calcoption, $bill_id;
    
    if ($knob == "Cancel")
    {
      $sql = "DELETE FROM bill_line WHERE id = ".$bill_line_id;
      $rec=$g_db->getrows($sql);
      $this->redirect();
    }
    else
    {
      include "./modules/finance/createbill.php";
    }
    
  }

  /* 
  function postUpdate($rec)
  {
     include "./modules/finance/createbill.php";
  }
  */
  
  function postAdd($rec)
  {
    $bill_line_id = $rec["id"];  
    if ($rec["calcoption"] == "discount" or $rec["calcoption"] == "costs")
    {
      include "./modules/finance/billaddon.php";
    }
    else
    {
      include "./modules/finance/createbill.php";
    }
  }
  
  function postDel($rec)
  {
    global $g_db;  

    if ($rec["calcoption"] == "fixed")
    {
	$sql = "UPDATE hours SET bill_line_id = '', registered = '', onbill = '', hour_rate = '' WHERE bill_line_id = ". $rec["id"];
	$xrec=$g_db->getrows($sql);
    }
    elseif ($rec["calcoption"] == "calc")
    {
      //query to empty the bill_line_id and registered fields from the hours table when you delete a bill_line.
      $sql = "UPDATE hours SET bill_line_id = '', registered = '', onbill = '', hour_rate = '' WHERE bill_line_id = ". $rec["id"];
      $xrec=$g_db->getrows($sql);
    }
    elseif ($rec["calcoption"] == "discount")
    {
      $sql = "DELETE FROM discount WHERE bill_line_id = ". $rec["id"];
      $xrec=$g_db->getrows($sql);
    }
    elseif ($rec["calcoption"] == "costs")
    {
      $sql = "UPDATE costregistration SET bill_line_id = '', costamount = '' WHERE bill_line_id = ". $rec["id"];
      $xrec=$g_db->getrows($sql);
    }
        
  }

}

?>
