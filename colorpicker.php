<?php
/* colorpicker.php
*
*  @Author : Rene Bakx (rene@ibuildings.nl)
*  @Version: $
*
* descr.  :  Show the colorpicker and returns the value to the the text field
*  input   :  $form.field     -> name of the form and field from wich the picker is called
*  call     : help.php?form=[form.field]
*/

  include("atk.inc");  
  
  atksession();
  atksecure();  
  require "theme.inc";
  
  
// makes ONE colored box
function drawTD($color)

{
 global $colWidth;
 global $colHeight;
 global $formRef;
 $row .="<td bgcolor='$color'>";
 $row .='<A class="picker" href ="javascript:remotePicker';
 $row .="('".$formRef."','".$color."')";
 $row .='"><IMG SRC="atk\images\dummy.gif" border=0 width='.$colWidth.' height='.$colHeight.' ></a></td>';
 $row .= "\n";
return $row;
}

//builds  the entire colorPicker Matrix table definition
function colorMatrix()
{
 global $usercol;
 $webColors = array ("66","77","88","99","AA","BB","CC","DD","EE","FF"); // values to mix, can be extended with more combo's

 $matrix .= '<table width="100%" border="1" cellspacing="0" cellpadding="0"><tr><td>';
 $matrix .= "\n";
 $matrix .= '<table width = "100%" border="0" cellspacing="2" cellpadding="0"><tr>';
 $matrix .="\n";
 for ($i=0;$i<count($webColors);$i++)
 // red
 { 
 $color = "#".$webColors[$i]."0000";
 $matrix  .= drawTD($color); 
 }
  // red -> green
 $j=count($webColors)-1;
 for ($i=0;$i<count($webColors);$i++)
 { 
 $color = "#".$webColors[$j].$webColors[$i]."00";
 $j--;
 $matrix  .= drawTD($color); 
 } 

 $matrix .="</tr><tr>\n";
 // green
 for ($i=0;$i<count($webColors);$i++)
 { 
 $color = "#00".$webColors[$i]."00";
 $matrix  .= drawTD($color); 
 }
   // green -> blue
 $j=count($webColors)-1;
 for ($i=0;$i<count($webColors);$i++)
 { 
 $color = "#00".$webColors[$j].$webColors[$i];
 $j--;
 $matrix  .= drawTD($color); 
 } 
 $matrix .="</tr><tr>\n";

 // blue
 for ($i=0;$i<count($webColors);$i++)
 { 
  $color = "#0000".$webColors[$i];
  $matrix  .= drawTD($color); 
 }

 // blue -> red
 $j=count($webColors)-1;
 for ($i=0;$i<count($webColors);$i++)
 { 
 $color = "#".$webColors[$i]."00".$webColors[$j];
 $j--;
 $matrix  .= drawTD($color); 
 } 

 $matrix .="</tr><tr>\n";
 // red & blue

 for ($i=0;$i<count($webColors);$i++)
 { 
  $color = "#".$webColors[$i]."00".$webColors[$i];
  $matrix  .= drawTD($color); 
 }
 
 // green & blue

 for ($i=0;$i<count($webColors);$i++)
 { 
  $color = "#00".$webColors[$i].$webColors[$i];
  $matrix  .= drawTD($color); 
 }
 $matrix .="</tr><tr>\n";

// red & green

 for ($i=0;$i<count($webColors);$i++)
 { 
  $color = "#".$webColors[$i].$webColors[$i]."00";
  $matrix  .= drawTD($color); 
 }

  if ($usercol =="")
  {
 // predefined user colors
   $color = "#000000";
   $matrix  .= drawTD($color);
   $color = "#333333";
   $matrix  .= drawTD($color);
   $color = "#666666";
   $matrix  .= drawTD($color);
   $color = "#999999";
   $matrix  .= drawTD($color);
   $color = "#CCCCCC";
   $matrix  .= drawTD($color);
   $color = "#FFFFFF";
   $matrix  .= drawTD($color);
   $color = "#FF0000";
   $matrix  .= drawTD($color);
   $color = "#00FF00";
   $matrix  .= drawTD($color);
   $color = "#0000FF";
   $matrix  .= drawTD($color);
   $color = "#0CAFE0";
   $matrix  .= drawTD($color);
 }

else
{
$userColors = explode("|",$usercol);
$nrCol = count($userColors);
if ($nrCol > 10) $nrCol = 10;
for ($i=0;$i<$nrCol;$i++)
  {
  $color = "#".$userColors[$i];
  $matrix  .= drawTD($color); 
  }
}
 
 $matrix .='</tr></table>';
 $matrix .= "\n";
 $matrix .="</td></tr></table>\n";

return $matrix;
}

 // builds matrix
   $colHeight = "15"; // height of each color element
   $colWidth = "15";   // width of each color element
   $formRef   = $form;
   $matrix = colorMatrix();
//  Display's the picker in the current ATK style-template
  $g_layout->output("<html>");
  $g_layout->head("ColorPicker");
  $g_layout->output('<script language="javascript" src="atk/javascript/colorpicker.js"></script>');
  $g_layout->body();
  $g_layout->output ($matrix);
  $g_layout->ui_bottom();
  $g_layout->output("</body>");
  $g_layout->output("</html>");
  $g_layout->outputFlush();
?>
