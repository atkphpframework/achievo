<?

/**
* This file get all the bill line information and then get all the billed hours, costs and discounts out of the database 
* and generates a nice Bill of it.
* Author: Marc van Agteren (Nov. 2001)
*/

global $g_db, $g_layout, $config_tax_symbol, $config_tax_rate, $config_currency_symbol;
global $contactinfo, $viewdate, $contactpers, $edit1;

$bill_expiredate = $this->m_postvars["bill_expiredate"];

//query to get the projectid out of the database
$sql = "SELECT 
	  projectid, status 
	FROM bill 
	WHERE id = ".$billid;

$projectid=$g_db->getrows($sql);

//query to get project and cutomer information.
$sql = "SELECT 
	  project.name,
	  project.fixed_price,
	  customer.id,
	  customer.name,
	  customer.address,
	  customer.zipcode,
	  customer.city,
	  customer.country
	FROM 
	  project, 
	  customer 
	WHERE 
	  project.id = ".$projectid[0]["projectid"];
$sql .= " AND project.customer = customer.id";
	  
$projectinfo=$g_db->getrows($sql);

//get all the contact person info
$sql = "SELECT
         id,
	 firstname,
	 lastname
	FROM
	  contact
	WHERE
	  company = ".$projectinfo[0]["id"];

$contactinfo=$g_db->getrows($sql);

if ($edit == 1)
{
  $sql = "SELECT contact_choose FROM bill WHERE id = ".$billid;
  $contactpers=$g_db->getrows($sql);
  $edit1 = $edit;
}

    function get_contacts()
    {
      global $contactinfo, $contactpers, $edit1;
      
      for ($x=0;$x<count($contactinfo);$x++)
      {
	if ($edit1 == 1)
	{
	  $contactname = $contactinfo[$x]["firstname"] . " " . $contactinfo[$x]["lastname"];
	  if ($contactpers["0"]["contact_choose"] == $contactname)
	  {
	    $select = "selected";
	    
	  }
	  else
	  {
	    $select = "";
	  }
	}

	$contacts.='<OPTION VALUE="'.$contactinfo[$x]["firstname"]." ".$contactinfo[$x]["lastname"].'" '." ".$select.'>'.$contactinfo[$x]["firstname"]." ".$contactinfo[$x]["lastname"].'</OPTION>';
      }
      return $contacts;
    }

//get all the bill line info
$sql = "SELECT
	  *
	FROM
	  bill_line
	WHERE
	  billid = ".$billid;

$bill_lineinfo=$g_db->getrows($sql);

for ($x=0;$x<count($bill_lineinfo);$x++)
{  
  $billlineid[] = $bill_lineinfo[$x]["id"];
  
  $bill_line_id = $bill_lineinfo[$x]["id"];
  //put calcoption in the calc_bill_line array
  $calcoption = $bill_lineinfo[$x]["calcoption"];
  $calc_bill_line[$bill_line_id]["calcoption"] = $calcoption;
  //put shortdescription in the calc_bill_line array
  $shortdescription = $bill_lineinfo[$x]["shortdescription"];
  $calc_bill_line[$bill_line_id]["shortdescription"] = $shortdescription;
  //put description in the calc_bill_line array
  $description = $bill_lineinfo[$x]["description"];
  $calc_bill_line[$bill_line_id]["description"] = $description;
  //put final description in the calc_bill_line array
  if ($edit == 1)
  {  
    $description_final = $bill_lineinfo[$x]["description_final"];
    $calc_bill_line[$bill_line_id]["description_final"] = $description_final;
  }
  
  if ($bill_lineinfo[$x]["calcoption"] == "fixed")
  {
    $value = $bill_lineinfo[$x]["fixed_billed"];
    $calc_bill_line[$bill_line_id]["value"] = $value;
  }
}

//checking if there are some bill lines
if ($billlineid == NULL)
{
  $g_layout->ui_top(text("title_error"));
  $g_layout->output('<BR>');
  $g_layout->output(text("bill_line_no"));
  $g_layout->output('<BR>');
  $g_layout->output('<A href="dispatch.php?atknodetype=finance.bill&atkaction">Back</A>');
  $g_layout->output('<BR><BR>');
}
elseif ($projectid["0"]["status"] == 'final')
{
  $g_layout->ui_top(text("title_error"));
  $g_layout->output('<BR>');
  $g_layout->output(text("bill_status_final"));
  $g_layout->output('<BR>');
  $g_layout->output('<A href="dispatch.php?atknodetype=finance.bill&atkaction">Back</A>');
  $g_layout->output('<BR><BR>');
}
else
{
	//query to get all the hours that are part of the bill lines.
	$sql = "SELECT
		  time,
		  bill_line_id,
		  registered,
		  onbill,
		  hour_rate
		FROM 
		  hours 
		WHERE 
		  bill_line_id IN (".implode(",",$billlineid).") ";
		  
	$hourinfo=$g_db->getrows($sql);
	
	//make a selection between the hours that are fixed and those which need to be calculated.
	for ($x=0;$x<count($hourinfo);$x++)
	{
	  if ($hourinfo[$x]["onbill"] == 1)
	  {
	    //calculation of the after-calculation hours.
	    $hour_value = ($hourinfo[$x]["time"] / 60) * $hourinfo[$x]["hour_rate"];
	    $hours = ($hourinfo[$x]["time"] / 60);
	    $bill_line_id = $hourinfo[$x]["bill_line_id"];
	    
	    $calc_bill_line[$bill_line_id]["value"] += $hour_value;
	    $calc_bill_line[$bill_line_id]["totalhours"] += $hours;
	  }
	}
	
	//query to get all the costs that are part of the bill lines.
	$sql = "SELECT
		  bill_line_id,
		  costamount
		FROM 
		  costregistration
		WHERE
		  bill_line_id IN (".implode(",",$billlineid).") ";
		  
	$costinfo=$g_db->getrows($sql);
	
	for ($x=0;$x<count($costinfo);$x++)
	{
	  $cost_value = $costinfo[$x]["costamount"];
	  $bill_line_id = $costinfo[$x]["bill_line_id"];
	  $calc_bill_line[$bill_line_id]["value"] += $cost_value;
	}
	
	//query to get all the discounts that are part of the bill lines.
	$sql = "SELECT
		  type,
		  amount,
		  bill_line_id,
		  apply_on
		FROM 
		  discount
		WHERE
		  bill_line_id IN (".implode(",",$billlineid).") ";
	
	$discountinfo=$g_db->getrows($sql);
	
	for ($x=0;$x<count($discountinfo);$x++)
	{
	  if ($discountinfo[$x]["apply_on"] == "bill")
	  {
	    $bill_line_id = $discountinfo[$x]["bill_line_id"];
	    $value = $discountinfo[$x]["amount"];
	    $type = $discountinfo[$x]["type"];
	    $calc_bill_line[$bill_line_id]["apply_on"] = "bill";
	    $calc_bill_line[$bill_line_id]["type"] = $type;
	    $calc_bill_line[$bill_line_id]["value"] = $value;
	  }
	  else
	  {
	    $bill_line_id = $discountinfo[$x]["bill_line_id"];
	    $apply_on = $discountinfo[$x]["apply_on"];
	    $value = $discountinfo[$x]["amount"];
	    $type = $discountinfo[$x]["type"];
	    $calc_bill_line[$bill_line_id]["apply_on"] = $apply_on;
	    $calc_bill_line[$bill_line_id]["type"] = $type;
	    $calc_bill_line[$bill_line_id]["value"] = $value;
	  }
	}
	
	//BEGINNING OF OUTPUT CODE
	
	$g_layout->ui_top(text("title_generatebill"));
	$g_layout->output('<form action="dispatch.php" method="post" name="generatebillform">');
	$g_layout->output(session_form());
	$g_layout->output('<input type="hidden" name="atknodetype" value="finance.bill">');
	$g_layout->output('<input type="hidden" name="atkaction" value="billonscreen">');
	$g_layout->output('<input type="hidden" name="bill_id" value="'.$billid.'">');
	    
	$g_layout->output('<BR><BR>');
	$g_layout->table_simple();
	$g_layout->output('<tr>');
	$g_layout->td('<B>'.text("billinfo_change").'<B><BR>','colspan="2"');
	$g_layout->output('</tr><tr>');
	$g_layout->td(text("bill_number"));
	  $today = getdate();
	  $year = $today['year'];
	  $bill_nr = $year.$billid;
	  $g_layout->td($bill_nr);
	  $g_layout->output('<INPUT type="hidden" name="bill_nr" value="'.$bill_nr.'">');
	$g_layout->output('</tr><tr>');
	$g_layout->td(text("bill_date"));
	  $bill_date = atkDateAttribute::formatDate($today, "d M Y", 0);
	$g_layout->td($bill_date);
	$g_layout->output('<INPUT type="hidden" name="bill_date" value="'.$bill_date.'">');
	$g_layout->output('</tr><tr>');
	  $now = mktime (0,0,0,$today["mon"],$today["mday"],$today["year"]);
	  $bill_expiredate = date("Y-m-d",$now+1209600);
	  $dummyrec = Array("bill_expiredate"=>array("year"=>substr($bill_expiredate,0,4), 
					       "month"=>substr($bill_expiredate,5,2),
					       "day"=>substr($bill_expiredate,8,2)));
	  $startdateatt = new atkDateAttribute("bill_expiredate","F d Y","d F Y", 0, date("YMd"));
	$g_layout->td(text("bill_expiredate"));
	$g_layout->td($startdateatt->edit($dummyrec).'<BR><BR>');
	$g_layout->output('</tr><tr>');
	$g_layout->td(text("choose_contact"));
	$g_layout->td('<SELECT name="select_contact">'.get_contacts().'</SELECT>');
	$g_layout->output('</tr><tr>');
	$g_layout->td(text("input_contact"));
	if ($edit == 1)
	{
	  $sql = "SELECT contact_type, customer_info, company_onbill FROM bill WHERE id = ".$billid;
	  $billinfo=$g_db->getrows($sql);	  
	}
	$g_layout->td('<INPUT type=text name="type_contact" value="'.$billinfo["0"]["contact_type"].'" >');
	$g_layout->output('</tr><tr>');
	$g_layout->td('<BR>','colspan="3"');
	$g_layout->output('</tr><tr>');
	$g_layout->td(text("customer_info"));
	if ($edit == 1)
	{
	  $g_layout->td('<TEXTAREA rows="6" cols="30" name="customerinfo">'.$billinfo["0"]["customer_info"].'</TEXTAREA>','colspan="1"');
	}
	else
	{
	  $g_layout->td('<TEXTAREA rows="6" cols="30" name="customerinfo">'.$projectinfo["0"]["name"]."\n".$projectinfo["0"]["address"]."\n".$projectinfo["0"]["zipcode"]." ".$projectinfo["0"]["city"]."\n".$projectinfo["0"]["country"].'</TEXTAREA>','colspan="1"');
	}  
	$g_layout->output('</tr><tr>');
	$g_layout->td(text("company_info"));
	if (($edit == 1) & ($billinfo["0"]["company_onbill"] == 1))
	{
	  $check = "checked";
	}
	else
	{
	  $check = "";
	}
	$g_layout->td('<INPUT type="checkbox" name="companyinfo" value="1"'.$check.'>'.text("company_info_checkbox"),'colspan="1"');
	$g_layout->output('</tr><tr>');
	$g_layout->td('<BR><BR><B>'.text("billlines_change").'<B><BR>','colspan="2"');
	$g_layout->td('<BR><BR><B>'.text("billlines_amount").'<B><BR>');
	$g_layout->output('</tr>');
	  
	  //display fixedbill_lines
	  $x=0;
	  foreach($calc_bill_line as $key => $value)
	  {
	    if ($value["calcoption"] == "fixed")
	    {
	    foreach($calc_bill_line as $discountvalue)
	      {
		if ($discountvalue["calcoption"] == "discount")
		{
		  if ($discountvalue["apply_on"] == $key)
		  {
		    $discount_type = $discountvalue["type"];
		    $discount_value = $discountvalue["value"];
		    if ($discountvalue["type"] == 1)
		    {
		      $display_discount = text("display_discount").$config_currency_symbol." ".$discountvalue["value"];
		    }
		    else
		    {	    
		      $display_discount = text("display_discount").$discountvalue["value"]."%";
		    }  
		  }
		}   
	      }
	      $x++;
	      $g_layout->output('<tr>');
	      $g_layout->td(text("Fixed_billline").$x.":");
	      if ($edit == 1)
	      {
	        $g_layout->td('<TEXTAREA rows="3" cols="40" name="fixed_description'.$x.'">'.$value["description_final"].'</TEXTAREA>');
	      }
	      else
	      {
	        $g_layout->td('<TEXTAREA rows="3" cols="40" name="fixed_description'.$x.'">'.$value["description"]."\n".$display_discount.'</TEXTAREA>');
	      }
	      $g_layout->td($config_currency_symbol." ".number_format($value["value"], 2,",","."));
	      $g_layout->output('<INPUT type="hidden" name="fixed'.$x.'" value="'.$value["value"].'">');
	      $g_layout->output('<INPUT type="hidden" name="fixed_discount_type'.$x.'" value="'.$discount_type.'">');
	      $g_layout->output('<INPUT type="hidden" name="fixed_discount_value'.$x.'" value="'.$discount_value.'">');
	      $g_layout->output('<INPUT type="hidden" name="fixed_id'.$x.'" value="'.$key.'">');
	      $g_layout->output('</tr>');
	      $display_discount = "";
	      $discount_value = "";
	      $discount_type = "";
	    }
	  }
	  $g_layout->output('<INPUT type="hidden" name="fixed_count" value="'.$x.'">');
	  
	  //display aftercalculation bill_lines
	  $x=0;
	  foreach($calc_bill_line as $key => $value)
	  {
	    if ($value["calcoption"] == "calc")
	    {
	      foreach($calc_bill_line as $discountvalue)
	      {
		if ($discountvalue["calcoption"] == "discount")
		{
		  if ($discountvalue["apply_on"] == $key)
		  {
		    $discount_type = $discountvalue["type"];
		    $discount_value = $discountvalue["value"];
		    if ($discountvalue["type"] == 1)
		    {
		      $display_discount = text("display_discount").$config_currency_symbol." ".$discountvalue["value"];
		    }
		    else
		    {	    
		      $display_discount = text("display_discount").$discountvalue["value"]."%";
		    }
		  }
		}   
	      }
	      $x++;
	      $g_layout->output('<tr>');
	      $g_layout->td(text("calc_billline").$x.":");
	      if ($edit == 1)
	      {
	        $g_layout->td('<TEXTAREA rows="3" cols="40" name="calc_description'.$x.'">'.$value["description_final"].'</TEXTAREA>');
	      }
	      else
	      {
	        $g_layout->td('<TEXTAREA rows="3" cols="40" name="calc_description'.$x.'">'.text("billed_hours").$value["totalhours"]." - ".$value["description"]."\n".$display_discount.'</TEXTAREA>');
	      }	
	      $g_layout->td($config_currency_symbol." ".number_format($value["value"], 2,",","."));
	      $g_layout->output('<INPUT type="hidden" name="calc'.$x.'" value="'.$value["value"].'">');
	      $g_layout->output('<INPUT type="hidden" name="calc_discount_type'.$x.'" value="'.$discount_type.'">');
	      $g_layout->output('<INPUT type="hidden" name="calc_discount_value'.$x.'" value="'.$discount_value.'">');
	      $g_layout->output('<INPUT type="hidden" name="calc_id'.$x.'" value="'.$key.'">');
	      $g_layout->output('</tr>');
	      $display_discount = "";
	      $discount_value = "";
	      $discount_type = "";
	    }  
	  }
	  $g_layout->output('<INPUT type="hidden" name="calc_count" value="'.$x.'">');
	  
	  //display aftercalculation bill_lines
	  $x=0;
	  foreach($calc_bill_line as $key => $value)
	  {
	    if ($value["calcoption"] == "costs")
	    {
	      $x++;
	      $g_layout->output('<tr>');
	      $g_layout->td(text("costs_billline").$x.":");
	      if ($edit == 1)
	      {
	        $g_layout->td('<TEXTAREA rows="3" cols="40" name="costs_description'.$x.'">'.$value["description_final"].'</TEXTAREA>');
	      }
	      else
	      {
	        $g_layout->td('<TEXTAREA rows="3" cols="40" name="costs_description'.$x.'">'.$value["description"].'</TEXTAREA>');
	      } 	
	      $g_layout->td($config_currency_symbol." ".number_format($value["value"], 2,",","."));
	      $g_layout->output('<INPUT type="hidden" name="costs'.$x.'" value="'.$value["value"].'">');
	      $g_layout->output('<INPUT type="hidden" name="costs_id'.$x.'" value="'.$key.'">');
	      $g_layout->output('</tr>');
	    }  
	  }
	  $g_layout->output('<INPUT type="hidden" name="costs_count" value="'.$x.'">');
	  
	  //display aftercalculation bill_lines
	  $x=0;
	  foreach($calc_bill_line as $key => $value)
	  {
	    if ($value["calcoption"] == "discount")
	    {
	      if ($value["apply_on"] == "bill")
	      {
		  $x++;
		  $g_layout->output('<tr>');
		  $g_layout->td(text("bill_discount"));
		  if ($value["type"] == 1) $g_layout->td("<B>".$config_currency_symbol." ".number_format($value["value"], 2,",",".")."</B> ".text("entire_bill_discount"));
		  if ($value["type"] == 2) $g_layout->td("<B>".$value["value"]."%</B>"." ".text("entire_bill_discount"));
		  $g_layout->output('<INPUT type="hidden" name="entire_discount_type'.$x.'" value="'.$value["type"].'">');
		  $g_layout->output('<INPUT type="hidden" name="entire_discount_value'.$x.'" value="'.$value["value"].'">');
		  $g_layout->output('<INPUT type="hidden" name="entire_id'.$x.'" value="'.$key.'">');
		  $g_layout->output('</tr>');
	      }
	    }  
	  }
	  $g_layout->output('<INPUT type="hidden" name="entirediscount_count" value="'.$x.'">');
	
	//$g_layout->output('</tr>');
	$g_layout->output('</table>');
	
	$g_layout->table_simple();
	$g_layout->output('<tr>');
	$g_layout->td('<BR><BR><input type="submit" value="'.text("generate_bill").'" >');
	$g_layout->output('</tr>');
	$g_layout->output("</FORM>");
	$g_layout->output('</table>');
}

?>