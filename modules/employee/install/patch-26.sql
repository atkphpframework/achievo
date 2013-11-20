 CREATE TABLE `tpl_usercontract` (
  `id` int(10) NOT NULL DEFAULT '0',
  `name` varchar(100) NOT NULL,
  `uc_hours` decimal(4,2) NOT NULL DEFAULT '0.00',
  `startdate` date NOT NULL DEFAULT '0000-00-00',
  `enddate` date DEFAULT NULL,
  `description` text,
  `workingdays` varchar(14) DEFAULT NULL,
  `workstarttime` time NOT NULL DEFAULT '00:00:00',
  `workendtime` time NOT NULL DEFAULT '00:00:00',
  PRIMARY KEY (`id`)
);