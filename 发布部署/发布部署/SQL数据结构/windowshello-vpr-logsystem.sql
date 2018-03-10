/*
Navicat MySQL Data Transfer

Source Server         : localhost
Source Server Version : 50612
Source Host           : 127.0.0.1:3306
Source Database       : windowshello-vpr-logsystem

Target Server Type    : MYSQL
Target Server Version : 50612
File Encoding         : 65001

Date: 2018-03-10 20:26:47
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
) ENGINE=InnoDB AUTO_INCREMENT=7 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of infolist
-- ----------------------------

-- ----------------------------
-- Table structure for userlist
-- ----------------------------
DROP TABLE IF EXISTS `userlist`;
CREATE TABLE `userlist` (
  `Username` varchar(255) NOT NULL,
  `Password` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of userlist
-- ----------------------------
INSERT INTO `userlist` VALUES ('admin', 'admin');
