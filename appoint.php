<?
  /* Setup the system */
  include_once("atk.inc");

  atksession();
  atksecure();
?>
<html>
<head>
 <title>Achievo scheduler item</title>
 <script language="JavaScript">
 <!--
  this.focus();
 //-->
 </script>
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

   include "scheduler/view_scheduler_item.inc";
?>
</body>
</html>