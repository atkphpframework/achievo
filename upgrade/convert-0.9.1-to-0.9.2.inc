<?php

  // TODO: this script uses installNode calls.
  // This is not allowed for conversion script, since the nodes refered to 
  // might disappear or change in later releases, leaving this conversion 
  // script crippled.
  // The calls must be replaced by create table and/or alter table statements.

  $setup->report("Start conversion of 0.9.1 database -> 0.9.2 database");
  
  atkdebug("In convert-0.9.1-to-0.9.2.inc");
  
  // Declare a utility function for converting varchar userid's to the new int
  // foreign keys for users.
  function useridConvert($table, $field, $setup)
  {
    static $s_users="";
    global $g_db;
    
    // One time retrieval of all employees.
    if ($s_users=="")
    {
      $sql = "SELECT id, userid FROM person WHERE role='employee'";
      $s_users = $g_db->getrows($sql);
    }
    
    if (count($s_users) > 0)
    {
      for ($i=0;$i<count($s_users);$i++)
      {                
        $g_db->query("UPDATE $table SET $field='".$s_users[$i]["id"]."' WHERE $field = '".$s_users[$i]["userid"]."'");
      }
    }    
    $setup->alterColumn($table, $field, $field, "int(10)", false);    
    
  }
  
  $setup->installNode("setup.versioninfo");
 
  // Create the schedule_notes table.
  $setup->installNode("calendar.schedule_notes");

  //convert contact table to person table and copy employees to person table
  $setup->renameTable("contact", "person");
  $setup->dropColumn("person", "owner"); // no longer used.
  
  // The following calls will add new fields to the person table.
  $setup->addColumn("person", "state", "varchar(100)");
  $setup->addColumn("person", "function", "varchar(50)");
  $setup->addColumn("person", "role", "varchar(15)", false);
  $setup->addColumn("person", "entity", "int(10)");
  $setup->addColumn("person", "supervisor", "int(10)");
  $setup->addColumn("person", "status", "varchar(9)");
  $setup->addColumn("person", "userid", "varchar(15)", false);
  $setup->addColumn("person", "password", "varchar(40)");
  
  $setup->addColumn("person", "theme", "varchar(15)");
  
  // Also update the usercontract node.
  $setup->addColumn("usercontract", "workingdays", "varchar(14)");
  
  // Create project_person table with all necessary fields.
  $setup->installNode("project.project_personcontact");
  $setup->installNode("project.project_personemployee");
 
  //set role = "contact" for all persons in person table
  //these persons are only contacts at this moment  
  $g_db->query("UPDATE person SET role = 'contact'");  

  //copy employees to persons table  
  $employees = $g_db->getrows("SELECT * FROM employee");

  if (count($employees)>0)
  {
    for ($j=0;$j<count($employees);$j++)
    {      
      $id = $g_db->nextid("person");
      
      // Assume names are in the format 'Jay Random Hacker'.
      // Firstname will become 'Jay', Lastname will become 'Random Hacker'.
      $names = explode(" ", $employees[$j]["name"]);
      $firstname = array_shift($names);
      $lastname = implode(" ", $names);
      
      $g_db->query("INSERT INTO person (id, 
                                        userid, 
                                        password, 
                                        lastname, 
                                        firstname, 
                                        email, 
                                        supervisor, 
                                        status, 
                                        theme, 
                                        entity, 
                                        role)
                                VALUES ('$id', 
                                        '".$employees[$j]["userid"]."', 
                                        '".$employees[$j]["password"]."', 
                                        '".$lastname."', 
                                        '".$firstname."',
                                        '".$employees[$j]["email"]."', 
                                        '".$employees[$j]["supervisor"]."', 
                                        '".$employees[$j]["status"]."', 
                                        '".$employees[$j]["theme"]."',
                                        '".$employees[$j]["entity"]."', 
                                        'employee')");      
    }

  }

  //drop employee table
  $setup->dropTable("employee");  

  //rename customer table to organization
  $setup->renameTable("customer", "organization");
    
  // Update the organization node with new fields.
  $setup->addColumn("organization", "state", "varchar(100)");

  $setup->installNode("project.role");  

  //insert roles in table role  
  $res = $g_db->query("INSERT INTO role (id, name) VALUES (1, 'Developer')");    
  $res = $g_db->query("INSERT INTO role (id, name) VALUES (2, 'Customer')");  
  $res = $g_db->query("INSERT INTO db_sequence (seq_name, nextid) VALUES ('role', 2)");  

  //copy contact of a project into project_person
  $sql = "SELECT project.* FROM project";
  $projects = $g_db->getrows($sql);
 
  $customer_defaultcontacts = array(); 
  
  for ($i=0;$i<count($projects);$i++)
  {  
    if ($projects[$i]["contact"] != 0)
    {     
      $g_db->query("INSERT INTO project_person (personid, projectid, role)
                         VALUES ('".$projects[$i]["contact"]."', '".$projects[$i]["id"]."', 2)");      
    }
    else
    {    
      if ($projects[$i]["customer"]!=0) // if customer is not set, we don't need to convert
      {
        // project has a customer, but a contact person was not set. Create a default contact person.        
        if (empty($customer_defaultcontacts[$projects[$i]["customer"]])) // check if we already added a default contact for this customer.
        {
        
          $id = $g_db->nextid("person");
          $g_db->query("INSERT INTO person (id, 
                                            firstname,
                                            lastname, 
                                            company, 
                                            role)
                                    VALUES ('$id', 
                                            '(please update)',
                                            'Auto-generated default contact', 
                                            '".$projects[$i]["customer"]."', 
                                            'contact')");      
          $customer_defaultcontacts[$projects[$i]["customer"]] = $id;          
        }
        $g_db->query("INSERT INTO project_person (personid, projectid, role)
                       VALUES ('".$customer_defaultcontacts[$projects[$i]["customer"]]."', '".$projects[$i]["id"]."', 2)");          
      }      
    }
  }
  
  $defaultcnt = count($customer_defaultcontacts);
  if ($defaultcnt>0)
  {  
    $setup->report("For ".$defaultcnt." customer".($defaultcnt>1?"s":"")." a 
                    new auto-generated contact person was added to be part of 
                    projects that did not have a contactperson yet. ".($defaultcnt>1?"These 
                    contacts have":"This contact has")." been named 'Auto-generated default 
                    contact'. Please update this name to the correct value.", "warning");
  }

  $setup->dropColumn("project", "contact");
  $setup->dropColumn("project", "customer");
  
  //add new fields to schedule table
  $setup->addColumn("schedule", "all_users", "int(1)");
  $setup->addColumn("schedule", "status", "varchar(11)");
  
  atkdebug("After last installnode");
     
  $setup->renameTable("schedule_attendees", "schedule_attendee");
  
  $setup->alterColumn("schedule_attendee", "scheduleid", "schedule_id", "int(10)", false);
  $setup->alterColumn("schedule_attendee", "userid", "person_id", "int(10)", false);        

  $setup->renameTable("schedule_types", "schedule_type");
    
  // employee_project records will be converted to project_person
  $sql = "SELECT employeeid FROM employee_project";
  $res = $g_db->getrows($sql);
  if (count($res) > 0)
  {
    for ($i=0;$i<count($res);$i++)
    {
      //search employee id with given userid      
      $result = $g_db->getrows("SELECT id FROM person WHERE userid ='".$res[$i]["employeeid"]."' AND role='employee'");
      $g_db->query("UPDATE employee_project SET employeeid='".$result[0]["id"]."' WHERE employeeid = '".$res[$i]["employeeid"]."'");
    }
  }

  //copy all employees to the project_person table  
  $projects = $g_db->getrows("SELECT * FROM employee_project");
  for ($i=0;$i<count($projects);$i++)
  {
    $g_db->query("INSERT INTO project_person (personid, projectid, role)
                       VALUES ('".$projects[$i]["employeeid"]."', '".$projects[$i]["projectid"]."', 1)");    
  }
  
  $setup->dropTable("employee_project");    
  
  //convert planning, employee_project, rate, usercontract, person, todo, todo_history and project table because this version
  //does not use userid as primary key for employees    
  useridConvert("hours", "userid", $setup);
  useridConvert("project", "coordinator", $setup);
  useridConvert("project_person", "personid", $setup);
  useridConvert("project_notes", "owner", $setup);
  useridConvert("usercontract", "userid", $setup);  
  useridConvert("person", "supervisor", $setup); 
  useridConvert("todo", "assigned_to", $setup);  
  useridConvert("todo", "owner", $setup);  
  useridConvert("todo_history", "assigned_to", $setup);
  useridConvert("todo_history", "owner", $setup);
  useridConvert("schedule", "owner", $setup);
  
  // Add new id field to hours_lock table.
  $setup->addColumn("hours_lock", "id", "int(10)", false);
 
  // We must give all records a unique value to be able to use the id
  // field as a primary key.
  $recs = $g_db->getrows("SELECT * FROM hours_lock");
  for ($i=0, $_i=count($recs); $i<$_i; $i++)
  {
    $g_db->query("UPDATE hours_lock 
                     SET id=".($i+1)." 
                   WHERE userid='".$recs[$i]["userid"]."' 
                     AND week='".$recs[$i]["week"]."'");
  }
  $res = $g_db->query("INSERT INTO db_sequence (seq_name, nextid) VALUES ('hours_lock', $i)");  
  
  $g_db->query("ALTER TABLE hours_lock DROP PRIMARY KEY, ADD PRIMARY KEY(id)");  
  $g_db->query("UPDATE hours_lock SET userid = '' WHERE userid='*'"); // We can't set it to null
                                                                      // yet, but we must remove
                                                                      // the '*' values to be
                                                                      // able to convert it to int
  useridConvert("hours_lock", "userid", $setup);    
  $setup->alterColumn("hours_lock", "userid", "userid", "int(10)", true); // for hours_lock, the userid field may be null  
  $g_db->query("UPDATE hours_lock SET userid = NULL WHERE userid=''");
  
  
  // Perform some additional alterations to make db more compatible
  // with fresh 0.9.2+ installs.
  $setup->alterColumn("accessright", "node", "node", "varchar(100)", false);
  $setup->alterColumn("accessright", "entity", "entity", "int(10)", false);
  $setup->alterColumn("activity", "id", "id", "int(10)", false);
  $setup->alterColumn("activity", "remarkrequired", "remarkrequired", "int(1)", true);
  $setup->alterColumn("activity", "description", "description", "varchar(50)", true);  
  $setup->alterColumn("db_sequence", "seq_name", "seq_name", "varchar(40)", false);
  $setup->alterColumn("db_sequence", "nextid", "nextid", "int(10)", false);
  $setup->alterColumn("hours", "id", "id", "int(10)", false);
  $setup->alterColumn("hours", "activityid", "activityid", "int(10)", false);
  $setup->alterColumn("hours", "phaseid", "phaseid", "int(10)", false);
  $setup->alterColumn("hours", "time", "time", "int(10)", false);
  //$setup->dropColumn("hours", "ratetype"); // no longer used.
  $setup->alterColumn("project", "id", "id", "int(10)", false);
  $setup->alterColumn("project", "status", "status", "varchar(15)", true);   
  $setup->alterColumn("project", "description", "description", "text", true);   
  $setup->alterColumn("project", "coordinator", "coordinator", "int(10)", true);   
  $setup->alterColumn("contract", "contracttype", "contracttype", "int(10)", false);
  $setup->alterColumn("contract", "billing_period", "billing_period", "varchar(12)", false);  
  $setup->alterColumn("contract", "id", "id", "int(10)", false);
  $setup->alterColumn("contract", "description", "description", "text", false);
  $setup->alterColumn("usercontract", "id", "id", "int(10)", false);
  $setup->alterColumn("contracttype", "id", "id", "int(10)", false);
  $setup->alterColumn("organization", "id", "id", "int(10)", false);
  $setup->alterColumn("organization", "address", "address", "varchar(100)", true);
  $setup->alterColumn("organization", "zipcode", "zipcode", "varchar(20)", true);
  $setup->alterColumn("organization", "city", "city", "varchar(100)", true);
  $setup->alterColumn("organization", "state", "state", "varchar(100)", true);
  $setup->alterColumn("organization", "country", "country", "varchar(100)", true);
  $setup->alterColumn("organization", "phone", "phone", "varchar(20)", true);
  $setup->alterColumn("organization", "fax", "fax", "varchar(20)", true);
  $setup->alterColumn("organization", "email", "email", "varchar(50)", true);
  $setup->alterColumn("organization", "remark", "remark", "text", true);
  $setup->alterColumn("organization", "bankaccount", "bankaccount", "varchar(30)", true);
  $setup->alterColumn("person", "id", "id", "int(10)", false);
  $setup->alterColumn("person", "firstname", "firstname", "varchar(50)", true);
  $setup->alterColumn("person", "lastname", "lastname", "varchar(50)", false);
  $setup->alterColumn("person", "supervisor", "supervisor", "int(10)", true);  
  $setup->alterColumn("person", "address", "address", "varchar(100)", true);
  $setup->alterColumn("person", "zipcode", "zipcode", "varchar(20)", true);
  $setup->alterColumn("person", "city", "city", "varchar(100)", true);
  $setup->alterColumn("person", "country", "country", "varchar(100)", true);
  $setup->alterColumn("person", "phone", "phone", "varchar(20)", true);
  $setup->alterColumn("person", "fax", "fax", "varchar(20)", true);
  $setup->alterColumn("person", "email", "email", "varchar(50)", true);
  $setup->alterColumn("person", "remark", "remark", "text", true);
  $setup->alterColumn("person", "company", "company", "int(10)", true);
  $setup->alterColumn("phase", "id", "id", "int(10)", false);
  $setup->alterColumn("phase", "status", "status", "varchar(9)", true); 
  $setup->alterColumn("phase", "max_phasetime", "max_phasetime", "int(10)", true); 
  $setup->alterColumn("phase", "max_hours", "max_hours", "int(10)", true); 
  $setup->alterColumn("phase", "description", "description", "text", true); 
  $setup->dropColumn("phase_activity", "billable"); // no longer used
  $setup->alterColumn("phase_activity", "phaseid", "phaseid", "int(10)", false);
  $setup->alterColumn("phase_activity", "activityid", "activityid", "int(10)", false);
  $setup->alterColumn("profile", "id", "id", "int(10)", false);
  $setup->alterColumn("project_notes", "id", "id", "int(10)", false);
  $setup->alterColumn("project_notes", "projectid", "projectid", "int(10)", false);
  $setup->alterColumn("project_notes", "description", "description", "text", false);
  $setup->alterColumn("schedule", "id", "id", "int(10)", false);
  $setup->alterColumn("schedule_type", "id", "id", "int(10)", false);
  $setup->alterColumn("schedule_note", "owner", "owner", "int(10)", false);
  $setup->alterColumn("schedule_note", "schedule_id", "schedule_id", "int(10)", false);
  $setup->alterColumn("todo", "id", "id", "int(10)", false);
  $setup->alterColumn("todo", "projectid", "projectid", "int(10)", false);
  $setup->alterColumn("todo", "status", "status", "int(1)", false);
  $setup->alterColumn("todo", "assigned_to", "assigned_to", "int(10)", true);
  $setup->alterColumn("todo", "priority", "priority", "int(1)", false);
  $setup->alterColumn("todo", "description", "description", "text", false);
  $setup->alterColumn("todo_history", "id", "id", "int(10)", false);
  $setup->alterColumn("todo_history", "todoid", "todoid", "int(10)", false);
  $setup->alterColumn("todo_history", "projectid", "projectid", "int(10)", false);
  $setup->alterColumn("todo_history", "description", "description", "text", false);
  $setup->alterColumn("todo_history", "status", "status", "int(1)", false);
  $setup->alterColumn("todo_history", "priority", "priority", "int(1)", false);
  $setup->alterColumn("todo_history", "assigned_to", "assigned_to", "int(10)", true);
  $setup->alterColumn("todo_history", "updated", "updated", "datetime ", true);
  $setup->alterColumn("tpl_phase_activity", "phaseid", "phaseid", "int(10)", false);
  $setup->alterColumn("tpl_phase_activity", "activityid", "activityid", "int(10)", false);
  $setup->alterColumn("tpl_phase", "description", "description", "text", true);
  $setup->alterColumn("tpl_phase", "id", "id", "int(10)", false);  
  $setup->alterColumn("tpl_project", "id", "id", "int(10)", false);  
  $setup->alterColumn("tpl_project", "description", "description", "text", true);  
 
  //change the node rights in the accessright table to this format: modulename.nodename
  $g_db->query("UPDATE accessright SET node = CONCAT('calendar.', node) WHERE node IN ('schedule', 'schedule_types', 'schedule_notes', 'schedule_attendees')");    
  $g_db->query("UPDATE accessright SET node = CONCAT('costreg.', node) WHERE node = 'costregistration'");  
  $g_db->query("UPDATE accessright SET node = CONCAT('employee.', node) WHERE node IN ('employee', 'profile', 'usercontracts', 'userprefs')");    
  $g_db->query("UPDATE accessright SET node = CONCAT('finance.', node) WHERE node IN ('rates', 'finance_customer', 'finance_project', 'billing_project', 'currency', 'bill', 'bill_line')");  
  $g_db->query("UPDATE accessright SET node = CONCAT('notes.', node) WHERE node IN ('project_notes', 'project_notesview')");    
  $g_db->query("UPDATE accessright SET node = 'organization.organization' WHERE node = 'customer'");    
  $g_db->query("UPDATE accessright SET node = CONCAT('organization.', node) WHERE node IN ('contact', 'contracts', 'contracttype')");    
  $g_db->query("UPDATE accessright SET node = CONCAT('project.', node) WHERE node IN ('project', 'phase', 'activity', 'tpl_phase', 'tpl_project')");  
  $g_db->query("UPDATE accessright SET node = 'reports.hoursurvey', action='report' WHERE node='hours' AND action='hoursurvey'");
  $g_db->query("UPDATE accessright SET node = CONCAT('reports.', node) WHERE node IN ('weekreport')");      
  $g_db->query("UPDATE accessright SET node = CONCAT('resource_planning.', node) WHERE node = 'resource_planning'");    
  $g_db->query("UPDATE accessright SET node = CONCAT('timereg.', node) WHERE node IN ('hours', 'hours_lock')");  
  $g_db->query("UPDATE accessright SET node = CONCAT('todo.', node) WHERE node = 'todo'");  
  
  // As of this release, we use version numbers. After running this conversion script, the 
  // database is fully up to date with version 1 of all modules.
  $setup->setVersion("1", "calendar");
  $setup->setVersion("1", "employee");
  $setup->setVersion("1", "notes");
  $setup->setVersion("1", "organization");
  $setup->setVersion("1", "person");
  $setup->setVersion("1", "pim");
  $setup->setVersion("1", "project");
  $setup->setVersion("1", "reports");
  $setup->setVersion("1", "search");
  $setup->setVersion("1", "setup");
  $setup->setVersion("1", "timereg");
  $setup->setVersion("1", "todo");  
  
?>