<?
global $g_layout, $g_db, $config_currency_symbol;

$g_layout->register_script("javascript/check.js");
$g_layout->register_script("javascript/formcheck.js");

  function get_activities($billid)
  {
    global $g_db;
    // Get the activities
    $sql = "SELECT id,shortdescription
            FROM bill_line
	    WHERE (calcoption = 'calc'
		    OR calcoption = 'fixed')
		  AND billid = ".$billid;  
    $sql .= " ORDER BY id";
    
    $records = $g_db->getrows($sql);
   	if($act_id==-1) { $sel="SELECTED"; } else { $sel=""; }
    $bill_line_code='<OPTION VALUE="bill" SELECTED>'.text("entirebill");
    for($i=0;$i<count($records);$i++)
    { 
      $bill_line_code.='<OPTION VALUE="'.$records[$i]["id"].'"'.$sel.'>'.$records[$i]["shortdescription"].'</OPTION>';
    }
    return $bill_line_code;
  }

//Add a new discount Bill_line
if ($rec["calcoption"] == "discount")
{
  $g_layout->ui_top(text("title_discount"));
  $g_layout->output('<form action="dispatch.php" method="post" name="discountform" onSubmit="return discountcheck()" >');
  $g_layout->output(session_form());
  $g_layout->output('<input type="hidden" name="atknodetype" value="finance.bill_line">');
  $g_layout->output('<input type="hidden" name="atkaction" value="adddiscount">');
  $g_layout->output('<input type="hidden" name="bill_line_id" value="'.$bill_line_id.'">');
  $g_layout->output('<input type="hidden" name="checktext" value="'.text("checktext").'">');
  $g_layout->output('<input type="hidden" name="checktext1" value="'.text("checktext1").'">');
  
  $g_layout->output('<BR><BR>');
  $g_layout->table_simple();
  $g_layout->output('<tr>');
  $g_layout->td('<B>'.text("specify_discount").'<B>','colspan="2"');
  $g_layout->output('</tr><tr>');
  $g_layout->td('<BR>','colspan="2"');
  $g_layout->output('</tr><tr>');
  $g_layout->td(text("type_discount"));
  $g_layout->td('<SELECT name="dis_option"><OPTION value="1">'.text("amountselect_discount").'</OPTION><OPTION value="2">'.text("percentage_discount").'</OPTION></SELECT>');
  $g_layout->output('</tr><tr>');
  $g_layout->td(text("amount_discount"));
  $g_layout->td('<input type="text" name="amount">');
  $g_layout->output('</tr><tr>');
  $g_layout->td(text("apply_discount"));
  $g_layout->td('<SELECT name="apply_on">'.get_activities($rec["billid"]["id"]).'</SELECT>');
  $g_layout->output('</tr>');
  $g_layout->output('</table>');
  
  $g_layout->table_simple();
  $g_layout->output('<tr>');
  $g_layout->td('<BR><input type="submit" value="'.text("addtobill").'" >');
  $g_layout->output('</tr>');
  $g_layout->output("</FORM>");
  $g_layout->output('</table>');
}

//Add a new costs bill_line
if ($rec["calcoption"] == "costs")
{
  $g_layout->ui_top(text("title_costsbill"));
  $g_layout->output('<form action="dispatch.php" method="post" name="billform">');
  $g_layout->output(session_form());
  $g_layout->output('<input type="hidden" name="atknodetype" value="finance.bill_line">');
  $g_layout->output('<input type="hidden" name="atkaction" value="addcosts">');
  $g_layout->output('<input type="hidden" name="bill_line_id" value="'.$bill_line_id  .'">');
  
  //query to get the projectid
  $sql = "SELECT projectid FROM bill where id = " .$rec["billid"]["id"];
  $projectrec=$g_db->getrows($sql);
  
  //query to get costsinformation
  $sql = "SELECT 
	    costregistration.id, 
	    costregistration.userid, 
	    costregistration.costdate, 
	    costregistration.value, 
	    costregistration.description,
	    currency.value as curvalue
	  FROM costregistration
	  LEFT JOIN currency ON costregistration.currency = currency.symbol
	  WHERE projectid = " .$projectrec["0"]["projectid"];
  $sql.= " AND costregistration.bill_line_id = 0";
  
  $costrec=$g_db->getrows($sql);
  
  for ($i=0;$i<count($costrec);$i++)
  {
    if ($costrec[$i]["curvalue"] > 0)
    {
      $amount = $costrec[$i]["value"] / $costrec[$i]["curvalue"];
    }
    else
    {
      $amount = $costrec[$i]["value"];
    }    
    $costrec[$i]["defvalue"] += $amount;
  }
  
  atk_var_dump($costrec);
 
  $g_layout->output('<br><b></b><br>');
  $g_layout->output($g_layout->data_top());

  $g_layout->output($g_layout->tr_top());
  $g_layout->td_datatitle(text("hours_date"));
  $g_layout->td_datatitle(text("hours_owner"));
  $g_layout->td_datatitle(text("hours_remark"));
  $g_layout->td_datatitle(text("costs_value"));
  $g_layout->td_datatitle(text("hours_register"));
  $g_layout->td("");
  $g_layout->output($g_layout->tr_bottom());

  $x=1;
  for ($i=0;$i<count($costrec);$i++)
  {
    $g_layout->output($g_layout->tr_top());
    $g_layout->td($costrec[$i]["costdate"]);
    $g_layout->td($costrec[$i]["userid"]);
    $g_layout->td($costrec[$i]["description"]);
    $g_layout->td("$config_currency_symbol ". round($costrec[$i]["defvalue"],2));
    $g_layout->td("<CENTER>".'<input type="checkbox" name="checkA'.$x.'" value="'.$costrec[$i]["id"].'" >'."<CENTER>");
    $g_layout->output('<input type="hidden" name="costamount'.$x.'" value="'.$costrec[$i]["defvalue"].'">');
    $g_layout->td("");
    $g_layout->output($g_layout->tr_bottom());
    $x++;
  }  

  $g_layout->output('<input type="hidden" name="costcount" value="'.$x.'">');
  $g_layout->output($g_layout->tr_top());
  
  for ($i=0;$i<6;$i++)
  {
    $g_layout->td("");
  }
  $g_layout->output($g_layout->tr_bottom());

  $g_layout->output($g_layout->tr_top());
  $g_layout->td("");
  $g_layout->td("");
  $g_layout->td("");
  $g_layout->td("");
  $g_layout->td("<CENTER>".'<input type="checkbox" name="A" onClick="checkC()"'."<CENTER>");
  $g_layout->td("(Un)Check All");
  $g_layout->output($g_layout->tr_bottom());
  $g_layout->output($g_layout->data_bottom());
  
  $g_layout->table_simple();
  $g_layout->output('<tr>');
  $g_layout->td('<BR><input type="submit" value="'.text("Add to Bill").'">');
  $g_layout->output('</tr>');
  $g_layout->output("</FORM>");
  $g_layout->output('</table>');

}

?>