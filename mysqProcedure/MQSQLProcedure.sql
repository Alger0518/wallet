DELIMITER ;;

DROP PROCEDURE IF EXISTS create_table_test; 
CREATE PROCEDURE create_table_test()
BEGIN
DROP TABLE IF EXISTS`address_test`;
CREATE TABLE IF NOT EXISTS  `address_test`(
`id` tinyint(4) unsigned NOT NULL AUTO_INCREMENT,
`messagecode` tinyint(4) unsigned NOT NULL,
`cointype` tinyint(4) NOT NULL,
`state` tinyint(4) NOT NULL,
`uid` int(11) NOT NULL DEFAULT '0',
`address` varchar(50) NOT NULL DEFAULT '',
`createtime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  KEY `TXIDINDEX` (`id`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS`deposit_test`;
CREATE TABLE IF NOT EXISTS  `deposit_test`(
`id` tinyint(4) unsigned NOT NULL AUTO_INCREMENT,
`messagecode` tinyint(4) unsigned NOT NULL,
`cointype` tinyint(4) NOT NULL,
`state` tinyint(4) NOT NULL,
`uid` int(11) NOT NULL DEFAULT '0',
`depositid` int(11) NOT NULL,
`amount` decimal(30,10) NOT NULL DEFAULT '0',
`createtime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
`address` varchar(50) NOT NULL DEFAULT '',
  KEY `TXIDINDEX` (`id`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS`balance_test`;
CREATE TABLE IF NOT EXISTS  `balance_test`(
`id` tinyint(4) unsigned NOT NULL AUTO_INCREMENT,
`messagecode` tinyint(4) unsigned NOT NULL,
`cointype` tinyint(4) NOT NULL,
`amount` decimal(30,10) NOT NULL DEFAULT '0',
`createtime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  KEY `TXIDINDEX` (`id`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

END;;

DELIMITER ;
