/*************************************************begin-创建btc系列存储过程*/
DROP PROCEDURE IF EXISTS create_btc_tables;
DELIMITER ;; 
CREATE PROCEDURE create_btc_tables(
    IN _tableName varchar(20),
    IN _coinType tinyint(4)
)  
BEGIN  
DECLARE `@suffix` VARCHAR(20);  
DECLARE `@sqlstr` VARCHAR(500);  
SET `@suffix` = _tableName;  
/*************************************************创建address_pool表*/
SET @sqlstr = CONCAT(  
"CREATE TABLE ",  
`@suffix`,
'_address_pool',
"(  
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `address` varchar(50) NOT NULL DEFAULT '',
  `uid` int(11) NOT NULL DEFAULT '0',
  `used` tinyint(4) NOT NULL DEFAULT '0' COMMENT '0 is unused and 1 is used.',
  `update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `ADDRESS` (`address`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;"  
);  
PREPARE stmt FROM @sqlstr;  
EXECUTE stmt;  
/*************************************************创建deposit表*/
SET @sqlstr = CONCAT(  
"CREATE TABLE ",  
`@suffix`,
'_deposit',
"(  
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `txid` varchar(255) NOT NULL DEFAULT '',
  `vout` int(11) NOT NULL DEFAULT '0',
  `address` varchar(50) NOT NULL DEFAULT '',
  `uid` int(11) NOT NULL DEFAULT '0',
  `amount` double NOT NULL DEFAULT '0',
  `cointype` tinyint(4) NOT NULL DEFAULT ",_coinType,",
  `confirmstate` tinyint(4) NOT NULL DEFAULT '0' COMMENT '0 is 6>confirms>=1; 1 is confirms>=6; 2 is discard',
  `update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `ADDRESS` (`address`),
  KEY `TXID` (`txid`),
  KEY `CONFIRM` (`confirmstate`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;"  
);  
PREPARE stmt FROM @sqlstr;  
EXECUTE stmt;

END;;
/*************************************************end-创建btc系列存储过程*/
/*************************************************begin-创建eth系列存储过程*/
DROP PROCEDURE IF EXISTS create_eth_tables;
CREATE PROCEDURE create_eth_tables(
    IN _tableName varchar(20),
    IN _coinType tinyint(4)
)  
BEGIN  
DECLARE `@suffix` VARCHAR(20);  
DECLARE `@sqlstr` VARCHAR(500);  
SET `@suffix` = _tableName;  
/*************************************************创建address_pool表*/
SET @sqlstr = CONCAT(  
"CREATE TABLE ",  
`@suffix`,
'_address_pool',
"(  
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `address` varchar(50) NOT NULL DEFAULT '',
  `uid` int(11) NOT NULL DEFAULT '0',
  `used` tinyint(4) NOT NULL DEFAULT '0' COMMENT '0 is unused and 1 is used.',
  `balance` double NOT NULL DEFAULT '0' COMMENT '0 is unused and 1 is used.',
  `nonce` BIGINT(4) NOT NULL DEFAULT '0' COMMENT '0 is unused and 1 is used.',
  `update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `ADDRESS` (`address`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;"  
);  
PREPARE stmt FROM @sqlstr;  
EXECUTE stmt;  
/*************************************************创建deposit表*/
SET @sqlstr = CONCAT(  
"CREATE TABLE ",  
`@suffix`,
'_deposit',
"(  
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `txid` varchar(255) NOT NULL DEFAULT '',
  `vout` int(11) NOT NULL DEFAULT '0',
  `address` varchar(50) NOT NULL DEFAULT '',
  `uid` int(11) NOT NULL DEFAULT '0',
  `amount` decimal(30,10) NOT NULL DEFAULT '0' COMMENT 'coin unit is eth ,NOT wei',
  `cointype` tinyint(4) NOT NULL DEFAULT ",_coinType,",
  `confirmstate` tinyint(4) NOT NULL DEFAULT '0' COMMENT '0 is 110>confirms>=90; 1 is confirms>=110; 2 is discard',
  `update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `ADDRESS` (`address`),
  KEY `TXID` (`txid`),
  KEY `CONFIRM` (`confirmstate`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;"  
);  
PREPARE stmt FROM @sqlstr;  
EXECUTE stmt;  

END;;
/*************************************************end-创建eth系列存储过程*/
/*************************************************创建表coin_type存储过程*/
DROP PROCEDURE IF EXISTS create_table_coin_type; 
CREATE PROCEDURE create_table_coin_type()
BEGIN
DROP TABLE IF EXISTS`coin_type`;
CREATE TABLE IF NOT EXISTS  `coin_type`(
`id` tinyint(4) unsigned NOT NULL,
`name` varchar(50) NOT NULL DEFAULT '' COMMENT 'such as eth,btc etc,used for prefix for data table!',
`engine` varchar(50) NOT NULL DEFAULT '' COMMENT 'btc,eth; for btc serials and eth serials',
`allow_transact_confirms` int(11) NOT NULL DEFAULT '1' COMMENT 'allow transact confirmation',
`allow_withdraw_confirms` int(11) NOT NULL DEFAULT '1' COMMENT 'allow withdraw confirmation',
  KEY `TXIDINDEX` (`id`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

END;;
/*************************************************创建表coin_type数据插入的存储过程*/
DROP PROCEDURE IF EXISTS insert_coin_type; 
CREATE PROCEDURE insert_coin_type(
    IN _id tinyint(4),
    IN _name varchar(50),
    IN _engine varchar(50),
    IN _txconfirms int(11),
    IN _wdconfirms int(11)
)
BEGIN
insert into coin_type values(_id, _name, _engine, _txconfirms, _wdconfirms);
END;;
DELIMITER ;
