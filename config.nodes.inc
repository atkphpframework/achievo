<?php
  /* register nodes */
  registerNode("hours", array("admin","add","edit","delete","hoursurvey","view_all"));
  registerNode("customer", array("admin", "add", "edit", "delete"));
  registerNode("contact", array("add","edit","delete"));
  registerNode("project", array("*admin", "add", "edit", "delete", "stats","planning"));
  registerNode("phase", array("add", "edit", "delete"));
  registerNode("userprefs", array("edit"));
  registerNode("activity", array("admin", "add", "edit", "delete", "stats"));
  registerNode("employee", array("admin", "add", "edit", "delete", "stats", "view_all"));
  registerNode("profile", array("admin", "add", "edit", "delete"));
  registerNode("tpl_phase", array("admin", "add", "edit", "delete"));
  registerNode("tpl_project", array("admin", "add", "edit", "delete"));
?>