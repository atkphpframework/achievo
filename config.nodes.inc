<?php
  /* register nodes */
  registerNode("hours", array("admin","add","edit","delete","lock","unlock","hoursurvey","view_all")); // niet
  registerNode("hours_lock", array("admin","add","delete")); // niet
  registerNode("userprefs", array("edit"));
  registerNode("weekreport",array("report","view_all"));
  registerNode("customer", array("admin", "add", "edit", "delete")); // niet
  registerNode("contact", array("add","edit","delete"));
  registerNode("contracts", array("admin","add","edit","delete"));
  registerNode("contracttype", array("admin","add","edit","delete"));

  registerNode("profile", array("admin", "add", "edit", "delete"));
  registerNode("employee", array("admin", "add", "edit", "delete", "stats", "view_all"));
  registerNode("usercontracts", array("admin", "add", "edit", "delete"));

  registerNode("project", array("admin", "add", "edit", "delete", "stats","planning"));
  registerNode("phase", array("add", "edit", "delete"));
  registerNode("activity", array("admin", "add", "edit", "delete", "stats"));
  registerNode("tpl_phase", array("admin", "add", "edit", "delete"));
  registerNode("tpl_project", array("admin", "add", "edit", "delete"));

  registerNode("todo", array("admin", "add", "edit", "delete"));
  registerNode("project_notes", array("admin", "add", "edit", "delete"));

  registerNode("project_notesview", array("admin", "add", "edit", "delete"));
  registerNode("holidays", array("admin","add","edit","delete"));

  registerNode("holidays", array("admin","add","edit","delete"));

  registerNode("schedule", array("admin", "add", "edit", "delete"));
  registerNode("schedule_types", array("admin", "add", "edit", "delete"));
  registerNode("costregistration", array("admin", "add", "edit", "delete", "costreport", "view_all"));

  registerNode("employee_project", array("admin", "add", "edit", "delete"));
?>
