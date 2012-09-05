<?php

function pimQuotation()
{
    $userid = atkGetUserId();
    $rl = &atknew("atk.recordlist.atkrecordlist");

    $qnode = &atkGetNode("quotation.quotation");
    $qnode->m_flags |= NF_NO_SEARCH;
    $qnode->m_action="admin";

    $actions = array();
    $actions["edit"] = dispatch_url("quotation.quotation","edit",array("atkselector"=>"[pk]"));

    $where = "quotation_quotation.status IN ('new','sent','called')
              AND quotation_quotation.assigned_to='".$userid."'";
    $recs = $qnode->selectDb($where,"","","",array("id","quotation_nr","title","organization_id","expire_date","status","price","profit_expectance"));
    $ret = atktext("pim_open_quotations").":<br>";
    if (count($recs)>0)
    {
      $ret.= $rl->render($qnode,$recs,$actions,RL_NO_EXTENDED_SEARCH|RL_NO_SEARCH|RL_NO_SORT, array("assigned_to"),"userleads",array(),"userleads");
    }
    else
    {
      $ret.= atktext("pim_no_quotations")."<br>";
    }
    $ret.= '<br>'.href(dispatch_url("quotation.quotation","add",array("atkfilter"=>"assigned_to.id='$userid'")),
                        atktext('add_quotation'),SESSION_NESTED);
    return $ret;
}

function pimMyPipeline()
{
  global $ATK_VARS;
  useattrib("graph.dataGraphAttribute");
  $starttime = time();

  if(is_array($ATK_VARS["startdate"]))
  {
    $starttime = mktime(0,0,0,$ATK_VARS["startdate"]["month"],$ATK_VARS["startdate"]["day"],$ATK_VARS["startdate"]["year"]);
  }
  $endtime = strtotime("+6 months",$starttime);
  if(is_array($ATK_VARS["enddate"]))
  {
    $endtime = mktime(0,0,0,$ATK_VARS["enddate"]["month"],$ATK_VARS["enddate"]["day"],$ATK_VARS["enddate"]["year"]);
  }

  $states = array("new","sent","called","accepted","rejected","prepaid","prepaidcompleted", "completed","billed");
  $optionArray=array();
  $valueArray=array();
  $initial_status=array();
  foreach($states as $status)
  {
    $optionArray[]=atktext($status,"quotation");
    $valueArray[]=$status;
    $initial_status[$status]=1;
  }

  if(is_array($ATK_VARS["status"]))
  {
    $initial_status = array();
    foreach($ATK_VARS["status"] as $key=>$value)
    {
      $initial_status[$value]=$value;
    }
  }

  $dummyrec = array(
        "startdate"=>array("year"=>date("Y",$starttime),
        "month"=>date("m",$starttime),
        "day"=>date("d",$starttime)),
        "enddate"=>array("year"=>date("Y",$endtime),
        "month"=>date("m",$endtime),
        "day"=>date("d",$endtime)),
        "status"=>$initial_status);

  $ret='<div id="pipelineadvanced" name="pipelineadvanced" style="display: none;">';
  $ret.='Advanced Search<br><br>';
  useattrib("atkdateattribute");
  useattrib("atkmultiselectattribute");
  $startdateatt = new atkDateAttribute("startdate","F d Y","d F Y");
  $enddateatt = new atkDateAttribute("enddate","F d Y","d F Y");
  $statusatt = new atkMultiSelectAttribute("status",$optionArray,$valueArray,5);

  $ret.='<form action="'.getDispatchFile().'" method="get" name="entryform">';

  $ret.=session_form();
  $ret.="Startdate: ".$startdateatt->edit($dummyrec)."<br>";
  $ret.="Enddate: ".$enddateatt->edit($dummyrec)."<br>";
  $ret.="Status: ".$statusatt->edit($dummyrec)."<br>";
  $ret.='<input type="submit" value="'.atktext("refresh").'">';
  $ret.="</form>";
  $ret.="<br><br></div>";
  $ret.='<a href="javascript:void(0)" onclick="toggleDisplay(\'pipelineadvanced\',get_object(\'pipelineadvanced\'))">Advanced search</a><br><br>';


  $params["resolution"] = "auto";
  $params["startdate"] = date("Y-m-d",$starttime);
  $params["enddate"] = date("Y-m-d",$endtime);
  $params["status"] = $initial_status;

  $cmppipeline= &new dataGraphAttribute("my_pipeline", "cmpmypipeline", $params, "bar");
  $cmppipeline->setSource("quotation.quotation");
  $ret.=$cmppipeline->showGraph();

  return $ret;

}
?>