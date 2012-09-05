ALTER TABLE `organization` CHANGE `address` `visit_address` VARCHAR( 100 ) CHARACTER SET latin1 COLLATE latin1_bin NULL DEFAULT NULL ,
CHANGE `address2` `visit_address2` VARCHAR( 100 ) CHARACTER SET latin1 COLLATE latin1_bin NULL DEFAULT NULL ,
CHANGE `zipcode` `visit_zipcode` VARCHAR( 20 ) CHARACTER SET latin1 COLLATE latin1_bin NULL DEFAULT NULL ,
CHANGE `city` `visit_city` VARCHAR( 100 ) CHARACTER SET latin1 COLLATE latin1_bin NULL DEFAULT NULL ,
CHANGE `state` `visit_state` VARCHAR( 100 ) CHARACTER SET latin1 COLLATE latin1_bin NULL DEFAULT NULL ,
CHANGE `country` `visit_country` VARCHAR( 100 ) CHARACTER SET latin1 COLLATE latin1_bin NULL DEFAULT NULL