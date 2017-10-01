/*
Navicat MySQL Data Transfer

Source Server         : MyPad
Source Server Version : 50617
Source Host           : 192.168.155.3:3306
Source Database       : windowshello-vpr-logsystem

Target Server Type    : MYSQL
Target Server Version : 50617
File Encoding         : 65001

Date: 2017-09-30 21:27:46
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for infolist
-- ----------------------------
DROP TABLE IF EXISTS `infolist`;
CREATE TABLE `infolist` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `InfoType` varchar(255) NOT NULL,
  `IssueTime` datetime NOT NULL,
  `Information` varchar(500) NOT NULL,
  `FilePath` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of infolist
-- ----------------------------
