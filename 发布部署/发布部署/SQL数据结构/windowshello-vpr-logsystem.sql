/*
Navicat MySQL Data Transfer

Source Server         : Mysql
Source Server Version : 50714
Source Host           : localhost:3306
Source Database       : windowshello-vpr-logsystem

Target Server Type    : MYSQL
Target Server Version : 50714
File Encoding         : 65001

Date: 2018-03-13 20:42:01
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for infolist
-- ----------------------------
DROP TABLE IF EXISTS `infolist`;
CREATE TABLE `infolist` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `InfoType` bit(1) NOT NULL,
  `ClientType` varchar(255) NOT NULL,
  `IssueTime` datetime NOT NULL,
  `Information` varchar(500) NOT NULL,
  `Result` varchar(900) DEFAULT NULL,
  `ClientIP` varchar(36) DEFAULT NULL,
  `FilePath` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of infolist
-- ----------------------------

-- ----------------------------
-- Table structure for userlist
-- ----------------------------
DROP TABLE IF EXISTS `userlist`;
CREATE TABLE `userlist` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `Username` varchar(255) NOT NULL,
  `Password` varchar(255) NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of userlist
-- ----------------------------
INSERT INTO `userlist` VALUES ('1', 'admin', 'admin');
