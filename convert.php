<?php
/*
* Achievo Convert Script 
* for converting the database of Achievo 0.4.x to Achievo 0.6
* See the doc/UPGRADE file for more detailed instructions on how
* to use this conversion script.
*
*/
// Settings Achievo 0.4.x database
$old_db_host = "localhost";
$old_db_name = "merp";
$old_db_user = "demo";
$old_db_passwd = "demo";

// The settings for the new Achievo 0.6 database must be entered
// in the config.inc.php3 file. 

// Password handling has changed in Achievo 0.6, so we cannot
// convert the old passwords. You can set a default password 
// here that will be assigned to each user.
// If you don't set this password you have to set them manually 
// for every user. Otherwise the user can login with the default 
// password and change it through the user preferences.
$default_user_passwd = "";

// If you set this to 0, no logfile will be generated.
// It is useful to leave this at 1 though. If you encounter errors
// during the conversion, you can mail the logfile to ivo@achievo.com
// to check out what the errors are.
$write_log = 1; // 0 = no  / 1 = yes
$log_file = "/tmp/achievo_convert.log";

// End of the settings......

// This script could take a while.. (if you encounter 'maximum execution
// time exceeded' errors, empty the new database, set this to a higher value
// and try again.
set_time_limit("180"); // 3 minutes should be enough.. 

//For the new database we use the config settings from achievo
include "atk/class.atknode.inc";

atksecure();

// Check of the administrator is logged in
if($g_user["name"]<>"administrator")
{
  echo "Sorry, only administrator can execute this script.";
  exit;
}


/*
* Small function to write logfile
*/
function write_log($msg)
{
  global $log_file, $write_log, $g_layout;
  
  $message="[".date("d-m-Y H:i:s")."] - ";
  $message.=$msg;
  $message.="\n";
 
  if($write_log==1)
  {
    $fp = fopen($log_file, "a");
    fputs($fp, strip_tags($message));
    fclose($fp);
  }
  $g_layout->output($message."<br>"); 
 
}

function handleError($query="")
{
  global $g_db;
  $error = $g_db->getError();
  
  if ($error!="")
  {
    $GLOBAL["errors"]=true;
    write_log('<font color="#ff0000">ERROR</font>: '.$error);
    if ($query!="")
    {
      write_log('Last query was: '.$query);
    }
  }
}

function update_sequence($seq_name,$new_number)
{
  global $g_db;
  $sql = "UPDATE db_sequence SET nextid=".$new_number."
          WHERE seq_name='".$seq_name."'";
  $res = $g_db->query($sql);
  handleError($sql);
}

$errors = false;

$g_layout->output("<html>");
$g_layout->head("Achievo Convert Script");
$g_layout->body();

if(!empty($convert)&&$convert==1)
{
  $g_layout->ui_top("Convert log");
  $g_layout->output('<div align="left">');
  write_log("Start conversion of 0.4.x database ($old_db_name) -> 0.6 ($config_databasename)");
  // make connection to the old database, for the new one, we use the achievo settings
  $old_db = @mysql_connect ($old_db_host, $old_db_user, $old_db_passwd)
          or die("Could not connect to old database $old_db_name (check username/password in convert.php3).");
  mysql_select_db($old_db_name, $old_db);
  write_log("Succesful connect to $old_db_name.");
  
  $sql = "INSERT INTO profile (id,name) VALUES (1,'Default group')";
  $res = $g_db->query($sql);
  handleError($sql);
  update_sequence("profile",2);  

  //First convert the user database
  $sql = "SELECT * 
          FROM employee";
  $result = @mysql_query ($sql,$old_db) or die ("Invalid query");
  $num_rows = @mysql_num_rows($result);
  write_log("Converting ".$num_rows." users......");
  while ($row = @mysql_fetch_array ($result)) 
  {
    if(strlen($default_user_passwd)>0)
    {
      $passwd = md5($default_user_passwd);      
    }
    else
    {
      $passwd = "";
    }
    $sql = "INSERT INTO employee (name,userid,email,password,status,theme,entity)
            VALUES ('".addslashes($row["name"])."','".addslashes($row["userid"])."','".addslashes($row["email"])."','".$passwd."','".$row["status"].
            "','achievo',1)";
    
    $res = $g_db->query($sql);
    handleError($sql);
  }  
  
  mysql_free_result ($result);
  
  // Convert the project, also create a main phase for every project
  $sql = "SELECT *
          FROM project";
  $result = @mysql_query ($sql,$old_db) or die ("Invalid query");
  $num_rows = @mysql_num_rows($result);
  write_log("Converting ".$num_rows." projects......");
  $id=0;
  while ($row = @mysql_fetch_array ($result)) 
  {
    $sql = "INSERT INTO project (id,name,coordinator,status,description,startdate,customer)
            VALUES ('".$row["id"]."','".addslashes($row["name"])."','".addslashes($row["coordinator"])."','".addslashes($row["status"]).
            "','".addslashes($row["description"])."','".date("Ymd")."','')";
    $res = $g_db->query($sql);
    handleError($sql);
  
    $sql = "INSERT INTO phase (id,name,projectid,status,description,max_phasetime,max_hours)
            VALUES ('".$row["id"]."','Main','".$row["id"]."','".$row["status"]."','Default phase',
            0,0)";
    $res = $g_db->query($sql);
    handleError($sql);
    if($row["id"]>$id) $id=$row["id"];        
  }
  update_sequence("project",$id+1);
  update_sequence("phase",$id+1);
  mysql_free_result ($result);
  
  // Convert the activities
  $sql = "SELECT *
          FROM activity";
  $result = @mysql_query ($sql,$old_db) or die ("Invalid query");
  $num_rows = @mysql_num_rows($result);
  write_log("Converting ".$num_rows." activities......");
  $id=0;
  while ($row = @mysql_fetch_array ($result)) 
  {
    $sql = "INSERT INTO activity (id,name,description,remarkrequired)
            VALUES ('".$row["id"]."','".addslashes($row["name"])."','".addslashes($row["description"])."','".$row["remarkrequired"]."')";
    $res = $g_db->query($sql); 
    handleError($sql);
    if($row["id"]>$id) $id=$row["id"];        
  }
  update_sequence("activity",$id+1);
  mysql_free_result ($result);
  
  //Convert the projectactivitys to phase activities
  $sql = "SELECT *
          FROM projectactivity ";
  $result = @mysql_query ($sql,$old_db) or die ("Invalid query");
  $num_rows = @mysql_num_rows($result);
  write_log("Converting ".$num_rows." projectactivities......");
  while ($row = @mysql_fetch_array ($result)) 
  {
    $sql = "INSERT INTO phase_activity (phaseid,activityid,billable)
            VALUES ('".$row["projectid"]."','".$row["activityid"]."','".$row["billable"]."')";
    $res = $g_db->query($sql); 
    handleError($sql);
  }
  mysql_free_result ($result);
  

  // Convert the hours
  $sql = "SELECT *
          FROM hours ";
  $result = @mysql_query ($sql,$old_db) or die ("Invalid query");
  $num_rows = @mysql_num_rows($result);
  write_log("Converting ".$num_rows." hourentries......");
  $id=0;
  while ($row = @mysql_fetch_array ($result)) 
  {
    $minutes = ($row["hours"]*60)+$row["minutes"];
    $sql = "INSERT INTO hours (id,entrydate,date,phaseid,activityid,remark,time,userid)
            VALUES ('".$row["id"]."','".$row["entrydate"]."','".$row["date"]."','".$row["projectid"]."','
            ".$row["activityid"]."','".addslashes($row["remark"])."','".$minutes."','".addslashes($row["userid"])."')";
    $res = $g_db->query($sql); 
    handleError($sql);
    if($row["id"]>$id) $id=$row["id"];        
  }
  update_sequence("hours",$id+1);
  mysql_free_result ($result);
  
  //Convert the templates, 
  $sql = "SELECT *
          FROM template ";
  $result = @mysql_query ($sql,$old_db) or die ("Invalid query");
  $num_rows = @mysql_num_rows($result);
  write_log("Converting ".$num_rows." templates......");
  $id=0;
  $counter=1;
  while ($row = @mysql_fetch_array ($result)) 
  {
    $sql = "INSERT INTO tpl_project (id,name,description)
            VALUES ('".$row["id"]."','".addslashes($row["name"])."','')";
    $res = $g_db->query($sql); 
    handleError($sql);

    $sql = "INSERT INTO tpl_project_phase (projectid,phaseid)
            VALUES ('".$row["id"]."','1')";
    $res = $g_db->query($sql);
    handleError($sql);

    $sql = "INSERT INTO tpl_phase (id,name,description)
           VALUES ('".$row["id"]."','Main ".$counter."','Default phase')";
    $res = $g_db->query($sql);
    handleError($sql);
    $counter++;
    if($row["id"]>$id) $id=$row["id"];        
  }
  update_sequence("tpl_project",$id+1);
  update_sequence("tpl_phase",$id+1);
  mysql_free_result ($result);

  //Convert the templates, create for every template a main phase
  $sql = "SELECT *
          FROM templateactivity ";
  $result = @mysql_query ($sql,$old_db) or die ("Invalid query");
  $num_rows = @mysql_num_rows($result);
  write_log("Converting ".$num_rows." template activities......");
  while ($row = @mysql_fetch_array ($result)) 
  {
    $sql = "INSERT INTO tpl_phase_activity (phaseid,activityid)
            VALUES ('".$row["templateid"]."','".$row["activityid"]."')";
    $res = $g_db->query($sql); 
    handleError($sql);
  }
  mysql_free_result ($result);

  mysql_close ($old_db);
  write_log("Conversion complete!");
  
  if ($errors==true)
  {
    $g_layout->output('<br><b>There were errors during the conversion!!</b>');
    $g_layout->output('<br><br>It can not be guaranteed that your new database contains all information of the old database.');
    $g_layout->output('Please mail the exact error messages to <a href="mailto:ivo@achievo.com">ivo@achievo.com</a>. We will try to find out what the problem is so you can try a new conversion.');
  }
  else
  {
    $g_layout->output('<br><a href="index.php3">Click here</a> to start Achievo!');
  }
  $g_layout->output('</div>');
}
else
{
  $g_layout->ui_top("Achievo Convert Script");
  $g_layout->output("This script will convert database <b>$old_db_name</b> to <b>".$config_databasename."</b>.<br>This could take a while for large databases.<br><br>Press the 'Convert' button to start the procedure.<br>");
  $g_layout->output('<form name="convert" action="convert.php3" method="post">');
  $g_layout->output('<input type="hidden" name="convert" value="1">');
  $g_layout->output('<input type="submit" name="submit" value="Convert">');

  $g_layout->output('</form>');
}
$g_layout->ui_bottom();
$g_layout->output("</body>");
$g_layout->output("</html>");
$g_layout->outputFlush();

?>
