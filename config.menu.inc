<?php
  /* menu items */
  
  // menuitem("-", "","main",true,372);
   
  

  menuitem("customermanagement",dispatch_url("customer", "admin"),"main",array("customer","admin"),500);
    menuitem("customer_admin", dispatch_url("customer", "admin"),"customermanagement", array("customer","admin"));
    menuitem("contracts", dispatch_url("contracts","admin"),"customermanagement", array("contracts","admin"));


  menuitem("setup","","main",true,700);    
     //menuitem("holiday_admin", dispatch_url("holidays", "admin"), "setup", array("activity", "admin"));
     menuitem("contracttype", dispatch_url("contracttype","admin"),"setup", array("contracttype","admin"));           
  menuitem("-","","main",true,720);         
  
  menuitem("support","","main",true,800);
     menuitem("achievo_docs", "manual.php", "support");
     menuitem("achievo_license", "license.php", "support");
     menuitem("achievo_home", "http://www.achievo.org/","support");  

?>
