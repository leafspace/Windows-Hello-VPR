package cn.leafspace.Database.Proxy;

import cn.leafspace.ToolBean.MessageItem;
import cn.leafspace.Database.Factory.DatabaseConnectorFactory;
import cn.leafspace.Database.Interface.DatabaseProxyInterface;
import cn.leafspace.Database.Interface.DatabaseConnectorInterface;

import java.sql.ResultSet;
import java.util.ArrayList;
import java.sql.SQLException;

public class MySqlDatabaseProxy implements DatabaseProxyInterface {
    private DatabaseConnectorInterface databaseConnection = null;

    public MySqlDatabaseProxy() {
        this.databaseConnection = new DatabaseConnectorFactory().getDatabaseConnection("MySQL");
    }

    public int findUser(String username, String password) {
        this.databaseConnection.getConnection();
        String sqlStr = "SELECT * FROM userlist WHERE Username = '" + username + "' AND `Password` = '" + password + "';";
        ResultSet resultSet = this.databaseConnection.query(sqlStr);
        try {
            while(resultSet.next()) {
                try {
                    return Integer.parseInt(resultSet.getString(1));
                } catch (NumberFormatException exception) {
                    return -1;
                }
            }
        } catch (SQLException e) {
            e.printStackTrace();
            return -1;
        } finally {
            this.databaseConnection.allClose();
        }
        return -1;
    }

    public ArrayList<MessageItem> getInfoList() {
        ArrayList<MessageItem> messageItems = new ArrayList<MessageItem>();
        this.databaseConnection.getConnection();
        String sqlStr = "SELECT * FROM infolist ORDER BY IssueTime DESC;";
        ResultSet resultSet = this.databaseConnection.query(sqlStr);
        try {
            while(resultSet.next()) {
                int ID = Integer.parseInt(resultSet.getString(1));
                boolean infoType = resultSet.getBoolean(2);
                String clientType = resultSet.getString(3);
                String issueTime = resultSet.getString(4);
                String information = resultSet.getString(5);
                String result = resultSet.getString(6);
                String clientIP = resultSet.getString(7);
                String filePath = resultSet.getString(8);
                messageItems.add(new MessageItem(ID, infoType, clientType, issueTime, information, result, clientIP, filePath));
            }
        } catch (SQLException e) {
            e.printStackTrace();
        } finally {
            this.databaseConnection.allClose();
        }
        return messageItems;
    }

    public ArrayList<MessageItem> getKeywordInfoList(String keyWord) {
        ArrayList<MessageItem> messageItems = new ArrayList<MessageItem>();
        this.databaseConnection.getConnection();
        String sqlStr = "SELECT * FROM infolist WHERE ClientType LIKE '%" + keyWord + "%' OR Information LIKE '%" +
                keyWord + "%' OR Result LIKE '%" + keyWord + "%' ORDER BY IssueTime DESC;";
        ResultSet resultSet = this.databaseConnection.query(sqlStr);
        try {
            while(resultSet.next()) {
                int ID = Integer.parseInt(resultSet.getString(1));
                boolean infoType = resultSet.getBoolean(2);
                String clientType = resultSet.getString(3);
                String issueTime = resultSet.getString(4);
                String information = resultSet.getString(5);
                String result = resultSet.getString(6);
                String clientIP = resultSet.getString(7);
                String filePath = resultSet.getString(8);
                messageItems.add(new MessageItem(ID, infoType, clientType, issueTime, information, result, clientIP, filePath));
            }
        } catch (SQLException e) {
            e.printStackTrace();
        } finally {
            this.databaseConnection.allClose();
        }
        return messageItems;
    }

    public MessageItem getMessage(int id) {
        MessageItem messageItems = null;
        this.databaseConnection.getConnection();
        String sqlStr = "SELECT * FROM infolist WHERE ID = "+ id +";";
        ResultSet resultSet = this.databaseConnection.query(sqlStr);
        try {
            while(resultSet.next()) {
                int ID = Integer.parseInt(resultSet.getString(1));
                boolean infoType = resultSet.getBoolean(2);
                String clientType = resultSet.getString(3);
                String issueTime = resultSet.getString(4);
                String information = resultSet.getString(5);
                String result = resultSet.getString(6);
                String clientIP = resultSet.getString(7);
                String filePath = resultSet.getString(8);
                messageItems = new MessageItem(ID, infoType, clientType, issueTime, information, result, clientIP, filePath);
                break;
            }
        } catch (SQLException e) {
            e.printStackTrace();
        } finally {
            this.databaseConnection.allClose();
        }
        return messageItems;
    }

    public void insertInfoItem(MessageItem messageItem) {
        this.databaseConnection.getConnection();
        String sqlStr = "INSERT INTO infolist(InfoType, ClientType, IssueTime, Information, Result, ClientIP, FilePath) VALUES("+
                messageItem.getInfoType() +", '"+ messageItem.getClientType() +"', '"+ messageItem.getIssueTime() +"', '"+
                messageItem.getInformation() +"', '"+ messageItem.getResult() +"', '"+ messageItem.getClientIP() +"', '"+ messageItem.getFilePath() +"');";
        this.databaseConnection.update(sqlStr);
        this.databaseConnection.allClose();
    }
}
