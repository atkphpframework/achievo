<?
  /* Setup the system */
  include_once("atk.inc");

  atksession();
  atksecure();
?>
<html>
<head>
 <title>Achievo scheduler item</title>
 <? if ($mode == 1) { ?>
 <script language="javascript" src="atk/javascript/open_appointment.js"></script>
 <? } ?>
</head>
<body topmargin="0" leftmargin="0" marginheight="0" marginwidth="0">

<? 
   // get appointment info
   $q = "SELECT
          a.*, b.item_description AS schedule_type
         FROM
          schedule a, schedule_types b
         WHERE 
          a.item_id=$id AND a.item_type=b.id";

   $appInfo = $g_db->getRows($q); 

   // get attendees
   $q       = "SELECT user_id FROM schedule_attendees WHERE schedule_id=$id";
   $attInfo = $g_db->getRows($q);

   if ($mode == 1)
   {
    include "layers.inc.php";
    include "scheduler/edit_scheduler_item.inc";
   }
   else
   {
    include "scheduler/view_scheduler_item.inc";
   }
?>
</body>
</html>