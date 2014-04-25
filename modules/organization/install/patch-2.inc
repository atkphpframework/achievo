<?php
// Because an old version of the install file did not set the patchlevel
// correctly, some upgrades might erroneously apply this patch.
// Therefor, I added a check so the patch is only executed if one of 
// the fields does not yet exist.
if (!$setup->columnExists("organization", "mail_address")) {
    $setup->addColumn("organization", "mail_address", "varchar(100)", true, "");
    $setup->addColumn("organization", "mail_zipcode", "varchar(20)", true, "");
    $setup->addColumn("organization", "mail_city", "varchar(100)", true, "");
    $setup->addColumn("organization", "mail_state", "varchar(100)", true, "");
    $setup->addColumn("organization", "mail_country", "varchar(100)", true, "");

    $db->query("UPDATE organization SET mail_address=address, 
                                          mail_zipcode=zipcode, 
                                          mail_city=city,
                                          mail_state=state,
                                          mail_country=country");
}
?>