<?php

class bill extends atkNode
{
  function bill()
  {
    $this->atkNode("bill", NF_EDITAFTERADD);
    $this->addAttribute(new atkAttribute("id" ,AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkManytoOneRelation("projectid","project", AF_OBLIGATORY));
    $this->addAttribute(new atkDateAttribute("create_date",AF_OBLIGATORY|AF_READONLY|AF_HIDE_ADD));
    $this->addAttribute(new atkAttribute("userid",AF_OBLIGATORY|AF_READONLY|AF_HIDE_ADD));
    $this->addAttribute(new atkTextAttribute("description", AF_HIDE_ADD));
    $this->addAttribute(new atkDummyAttribute("fixed","",AF_HIDE_ADD|AF_HIDE_LIST));
    
    $this->addAttribute(new atkOneToManyRelation("bill_line","finance.bill_line","billid",AF_HIDE_LIST|AF_CASCADE_DELETE));
    $genbill = text('generate_bill');
    $this->add(new atkParserAttribute("Bill","<A href='dispatch.php?billid=[id]&atknodetype=finance.bill&atkaction=generatebill'><FONT size=+1><B><CENTER>$genbill</CENTER></B></FONT></A><BR><BR>", AF_HIDE_LIST|AF_HIDE_ADD|AF_BLANKLABEL));

    $this->setTable("bill","bill");
    $this->setOrder("create_date,id desc");
    
    atkdebug("bill::bill()");
    $this->m_securityMap["billonscreen"] = "add";
    $this->m_securityMap["billsubmit"] = "add";
  }
  
  function initial_values()
  {
    global $g_user;
    $initial_date = Array("year"=>date("Y"), "month"=>date("m"), "day"=>date("d"));
    return array("create_date"=>$initial_date,"userid"=>strtolower($g_user["name"]));
  }
  
  function fixed_edit($rec)
  {
    global $g_db;
        
    $sql = "SELECT project.fixed_price FROM project where project.id = " . $rec["projectid"]["id"];
    $fixedrec=$g_db->getrows($sql);
    
    if ($fixedrec["0"]["fixed_price"] > 0)
    {
      $text = "<B>" . text("warning_fixedprice1") . round($fixedrec["0"]["fixed_price"],1) . "!</B>";
      sessionStore("fixed_price", $fixedrec["0"]["fixed_price"]);
      return $text;
    }
    else
    {
      sessionStore("fixed_price", "0");
    }
  
  }
  
  function action_generatebill()
  {
     global $billid;
     include "./modules/finance/generate_bill.php";
  }
  
  function action_billsubmit()
  {
     global $printknob, $editknob, $billid, $g_db;
     
     if ($editknob !="")
     {
       $edit = 1;
       include "./modules/finance/generate_bill.php";
     }
     else
     {
       $sql = "UPDATE bill SET status = 'final' WHERE id = ".$billid;
       $g_db->getrows($sql);
       $this->redirect();
     }     
     
  }
  
  function action_billonscreen()
  {
    global $g_layout, $config_tax_symbol, $config_tax_rate, $config_currency_symbol, $g_db, $bill_id;
    global $bill_nr, $bill_date, $bill_expiredate, $select_contact, $type_contact, $customerinfo, $companyinfo;
    global $fixed_count, $calc_count, $costs_count, $entirediscount_count;
    global $config_company_name, $config_company_adress, $config_company_zipcode, $config_company_city, $config_company_country;
    
    //Store bill information in the DB
    $sql = "UPDATE bill SET contact_choose = '".$select_contact."', contact_type = '".$type_contact;
    $sql .= "', customer_info = '".$customerinfo."', company_onbill = '".$companyinfo."', status = 'edit'";
    $sql .= " WHERE id = ".$bill_id;
    $put_in_db=$g_db->getrows($sql);
    
    //$g_layout->ui_top(text("title_generatebill"));
    $g_layout->output('<form action="dispatch.php" method="post" name="generatebillform">');
    $g_layout->output(session_form());
    $g_layout->output('<input type="hidden" name="atknodetype" value="finance.bill">');
    $g_layout->output('<input type="hidden" name="atkaction" value="billsubmit">');
    $g_layout->output('<input type="hidden" name="billid" value="'.$bill_id.'">');
    
    $g_layout->output('<BR><BR>');
    $g_layout->table_simple('0','true');
    $g_layout->output('<tr>');
    $g_layout->td('<DIV ALIGN="left"><FONT size="+2"><B>'.text("outputbill_bill").'<B></FONT><BR><BR></DIV>');
    $g_layout->output('</tr>');
    if ($companyinfo == 1)
    {
      $g_layout->output('<tr>');
      $g_layout->td('<DIV ALIGN="left">'.$config_company_name."<BR>".$config_company_adress."<BR>".$config_company_zipcode." ".$config_company_city."<BR>".$config_company_country."<BR><BR>".'</DIV>');
      $g_layout->output('</tr>');
    }      
    $g_layout->output('<tr>');
    $customerinfo = nl2br($customerinfo);
    $g_layout->td('<DIV ALIGN="left"><B>'.text("outputbill_billto")."</B><BR>".$customerinfo."<BR>".'</DIV>');
    $g_layout->output('</tr>');
    if ($type_contact != "")
    {
      $contact = $type_contact;
    }
    else
    {
       $contact = $select_contact;
    }
    $g_layout->output('<tr>');
    $g_layout->td('<DIV ALIGN="left">'.text("outputbill_contact")." ".$contact.'</DIV>');
    $g_layout->output('</tr>');
    $g_layout->output('</table>');
    $g_layout->output('<BR><BR>');
    
    $g_layout->table_simple('0','true');
    $g_layout->output('<tr>');
    $g_layout->td('<DIV ALIGN="left"><B>'.text("outputbill_billnr")."</B> ".'</DIV>', 'width="15%"');
    $g_layout->td('<DIV ALIGN="left">'.$bill_nr.'</DIV>');
    $g_layout->output('</tr>');
    $g_layout->output('<tr>');
    $g_layout->td('<DIV ALIGN="left"><B>'.text("outputbill_billdate")."</B> ".'</DIV>', 'width="15%"');
    $g_layout->td('<DIV ALIGN="left">'.$bill_date.'</DIV>');
    $g_layout->output('</tr>');
    $g_layout->output('<tr>');
    $month = date("M", mktime(0,0,0,$bill_expiredate[month],1,2000));
    $g_layout->td('<DIV ALIGN="left"><B>'.text("outputbill_billexpiredate")."</B> ".'</DIV>', 'width="15%"');
    $g_layout->td('<DIV ALIGN="left">'.$bill_expiredate[day]." ".$month." ".$bill_expiredate[year].'</DIV>');
    $g_layout->output('</tr>');
    $g_layout->output('</table>');
    $g_layout->output('<BR>');
    
    $g_layout->table_simple('0','true');
    $g_layout->output('<tr>');
    $g_layout->td('<DIV ALIGN="left"><B>'.text("outputbill_description").'</B></DIV>', 'width="70%"');
    $g_layout->td('<DIV ALIGN="left"><B>'.$config_tax_symbol."%".'</B></DIV>', 'width="10%"');
    $g_layout->td('<DIV ALIGN="left"><B>'.text("outputbill_amount").'</B></DIV>', 'width="20%"');
    $g_layout->output('</tr>');

    //making arrays of the diffrent bill lines
    for ($i=1;$i<=($fixed_count);$i++)
    {
      $description = "fixed_description".$i;
      $value = "fixed".$i;
      $discount_type = "fixed_discount_type".$i;
      $discount_value = "fixed_discount_value".$i;
      $id = "fixed_id".$i;

      $value1 = $GLOBALS[$description];
      $value2 = $GLOBALS[$value];
      $value3 = $GLOBALS[$discount_type];
      $value4 = $GLOBALS[$discount_value];
      $value5 = $GLOBALS[$id];
      if ($value2 != "")
      {     
        $fixedArr[$i]["description"] = $value1;
	$fixedArr[$i]["value"] = $value2;
	$fixedArr[$i]["discount_type"] = $value3;
	$fixedArr[$i]["discount_value"] = $value4;
	$fixedArr[$i]["id"] = $value5;
	
	//Store bill information in the DB
	$sql = "UPDATE bill_line SET description_final = '".$fixedArr[$i]["description"]."' WHERE id = ".$fixedArr[$i]["id"];
        $put_in_db=$g_db->getrows($sql);
      }
    }
    
    for ($i=1;$i<=($calc_count);$i++)
    {
      $description = "calc_description".$i;
      $value = "calc".$i;
      $discount_type = "calc_discount_type".$i;
      $discount_value = "calc_discount_value".$i;
      $id = "calc_id".$i;
 
      $value1 = $GLOBALS[$description];
      $value2 = $GLOBALS[$value];
      $value3 = $GLOBALS[$discount_type];
      $value4 = $GLOBALS[$discount_value];
      $value5 = $GLOBALS[$id];
      if ($value2 != "") 
      {     
        $calcArr[$i]["description"] = $value1;
	$calcArr[$i]["value"] = $value2;
	$calcArr[$i]["discount_type"] = $value3;
	$calcArr[$i]["discount_value"] = $value4;
	$calcArr[$i]["id"] = $value5;
	
	//Store bill information in the DB
	$sql = "UPDATE bill_line SET description_final = '".$calcArr[$i]["description"]."' WHERE id = ".$calcArr[$i]["id"];
        $put_in_db=$g_db->getrows($sql);
	
      }
    }
    
    for ($i=1;$i<=($costs_count);$i++)
    {
      $description = "costs_description".$i;
      $value = "costs".$i;
      $id = "costs_id".$i;
       
      $value1 = $GLOBALS[$description];
      $value2 = $GLOBALS[$value];
      $value5 = $GLOBALS[$id];
      if ($value2 != "") 
      {     
        $costsArr[$i]["description"] = $value1;
	$costsArr[$i]["value"] = $value2;
	$costsArr[$i]["id"] = $value5;
	
	//Store bill information in the DB
	$sql = "UPDATE bill_line SET description_final = '".$costsArr[$i]["description"]."' WHERE id = ".$costsArr[$i]["id"];
        $put_in_db=$g_db->getrows($sql);
      }
    }
    
    //sdfsdffsdsdf
    for ($i=1;$i<=($entirediscount_count);$i++)
    {
      $type = "entire_discount_type".$i;
      $value = "entire_discount_value".$i;
      $id = "entire_id".$i;
       
      $value1 = $GLOBALS[$type];
      $value2 = $GLOBALS[$value];
      if ($value1 != "") 
      {     
        $discountArr[$i]["type"] = $value1;
	$discountArr[$i]["value"] = $value2;
      }
    }
    
    $g_layout->output('<tr>');
    $g_layout->td('<HR width="100%">', 'colspan="3"');
    $g_layout->output('</tr>');
    
    for ($x=1;$x<=count($fixedArr);$x++)
    {
      $g_layout->output('<tr>');
      $g_layout->td('<DIV ALIGN="left">'.$fixedArr[$x]["description"].'</DIV>', 'width="70%"');
      $g_layout->td('<DIV ALIGN="left">'.$config_tax_rate.'</DIV>', 'width="10%" valign="top"');
      if ($fixedArr[$x]["discount_type"] == 1)
      {
        $fixedvalue = $fixedArr[$x]["value"] - $fixedArr[$x]["discount_value"];
      }
      elseif ($fixedArr[$x]["discount_type"] == 2)
      {
        $fixedvalue = $fixedArr[$x]["value"] - (($fixedArr[$x]["value"]/100)*$fixedArr[$x]["discount_value"]);
      }
      else
      {
	$fixedvalue = $fixedArr[$x]["value"];
      }
      $subtotal += $fixedvalue;
      $g_layout->td('<DIV ALIGN="left">'.$config_currency_symbol." ".$fixedvalue.'</DIV>', 'width="20%" valign="top"');
      $g_layout->output('</tr>');
      $lines++;
    }
    for ($x=1;$x<=count($calcArr);$x++)
    {
      $g_layout->output('<tr>');
      $g_layout->td('<DIV ALIGN="left">'.$calcArr[$x]["description"].'</DIV>', 'width="70%"');
      $g_layout->td('<DIV ALIGN="left">'.$config_tax_rate.'</DIV>', 'width="10%" valign="top"');
      if ($fixedArr[$x]["discount_type"] == 1)
      {
        $calcvalue = $calcArr[$x]["value"] - $calcArr[$x]["discount_value"];
      }
      elseif ($calcArr[$x]["discount_type"] == 2)
      {
        $calcvalue = $calcArr[$x]["value"] - (($calcArr[$x]["value"]/100)*$calcArr[$x]["discount_value"]);
      }
      else
      {
	$calcvalue = $calcArr[$x]["value"];
      }
      $subtotal += $calcvalue;
      $g_layout->td('<DIV ALIGN="left">'.$config_currency_symbol." ".$calcvalue.'</DIV>', 'width="20%" valign="top"');
      $g_layout->output('</tr>');
      $lines++;
    }
    for ($x=1;$x<=count($costsArr);$x++)
    {
      $subtotal += $costsArr[$x]["value"];
      $g_layout->output('<tr>');
      $g_layout->td('<DIV ALIGN="left">'.$costsArr[$x]["description"].'</DIV>', 'width="70%"');
      $g_layout->td('<DIV ALIGN="left">'.$config_tax_rate.'</DIV>', 'width="10%" valign="top"');
      $g_layout->td('<DIV ALIGN="left">'.$config_currency_symbol." ".$costsArr[$x]["value"].'</DIV>', 'width="20%" valign="top"');
      $g_layout->output('</tr>');
      $lines++;
    }
      $lines = $lines * 2;
      $g_layout->output('<tr>');
      $g_layout->td('<BR>', 'colspan="3" width="70%"');
      $g_layout->output('</tr>');
    
    for ($x=1;$x<=count($discountArr);$x++)
    {  
      //entire bill discount: amount
      if ($discountArr[$x]["type"] == 1)
      {
	$lines++;
        $g_layout->output('<tr>');
        $g_layout->td('<DIV ALIGN="left">'.$config_currency_symbol." ".$discountArr[$x]["value"]." ".text("outputbill_entirediscount").'</DIV>', 'width="70%"');
        $g_layout->td('<DIV ALIGN="left"></DIV>', 'width="10%"');
        $g_layout->td('<DIV ALIGN="left">'.$config_currency_symbol." ".round($discountArr[$x]["value"],2)." -".'</DIV>', 'width="20%" valign="top"');
        $g_layout->output('</tr>');      
        $subtotal -= $discountArr[$x]["value"];
      }
      //entire bill discount: %
      if ($discountArr[$x]["type"] == 2)
      {
	$lines++;
        $entire = ($subtotal / 100) * $discountArr[$x]["value"];
        $g_layout->output('<tr>');
        $g_layout->td('<DIV ALIGN="left">'.$discountArr[$x]["value"]."%"." ".text("outputbill_entirediscount").'</DIV>', 'width="70%"');
        $g_layout->td('<DIV ALIGN="left"></DIV>', 'width="10%"');
        $g_layout->td('<DIV ALIGN="left">'.$config_currency_symbol." ".round($entire,2)." -".'</DIV>', 'width="20%"');
        $g_layout->output('</tr>');
        $subtotal -= $entire;
      }
    }
      $g_layout->output('</table>');
    
    for ($i=0;$i<=(30-$lines);$i++)
    {
      $g_layout->output('<BR>');      
    }
      
    $taxtotal = (($subtotal / 100) * $config_tax_rate);
    $totalvalue = $taxtotal + $subtotal;
    
    $g_layout->table_simple('0','true');
    $g_layout->output('<tr>');
    $g_layout->td('', 'width="60%"');
    $g_layout->td('<DIV ALIGN="left">'.text("outputbill_totalexcl").$config_tax_symbol.'</DIV>', 'width="20%"');
    $g_layout->td('<DIV ALIGN="left">'.$config_currency_symbol." ".round($subtotal,2).'</DIV>', 'width="20%"');
    $g_layout->output('</tr>');
    $g_layout->output('<tr>');
    $g_layout->td('', 'width="60%"');
    $g_layout->td('<DIV ALIGN="left">'.$config_tax_symbol." ".$config_tax_rate."%".'</B></DIV>', 'width="20%"');
    $g_layout->td('<DIV ALIGN="left">'.$config_currency_symbol." ".round($taxtotal,2).'</DIV>', 'width="20%"');
    $g_layout->output('</tr>');
    $g_layout->output('<tr>');
    $g_layout->td('', 'width="60%"');
    $g_layout->td('<HR>', 'width="20%" colspan="2"');
    $g_layout->output('</tr>');
    $g_layout->output('<tr>');
    $g_layout->td('', 'width="60%"');
    $g_layout->td('<DIV ALIGN="left"><B>'.text("outputbill_totalpay").'</B></DIV>', 'width="20%"');
    $g_layout->td('<DIV ALIGN="left"><B>'.$config_currency_symbol." ".round($totalvalue,2).'</B></DIV>', 'width="20%"');
    $g_layout->output('</tr>');
    $g_layout->output('</table>');
    
    $g_layout->table_simple();
    $g_layout->output('<tr>');
    $g_layout->td('<BR><BR><input type="button" name="printknob" value="'.text("bill_print").'" onClick="javascript:window.print()"><input type="submit" name="editknob" value="'.text("bill_edit").'"><input type="submit" name="okknob" value="'."Ok".'">');
    $g_layout->output('</tr>');
    $g_layout->output("</FORM>");
    $g_layout->output('</table>');
   
  }
  
  function deleteDb($selector)
  {
   global $g_db;
     
   $record = $this->selectDb($selector);
     
   // call original delete
   atkNode::deleteDb($selector);
  }
 
}

?>
