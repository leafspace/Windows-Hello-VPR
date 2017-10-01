package cn.leafspace.Database.Proxy;

import cn.leafspace.ToolBean.MessageItem;
import cn.leafspace.Database.Factory.DatabaseConnectorFactory;
import cn.leafspace.Database.Interface.DatabaseProxyInterface;
import cn.leafspace.Database.Interface.DatabaseConnectorInterface;

import java.sql.ResultSet;
import java.util.ArrayList;
import java.sql.Connection;
import java.sql.SQLException;

public class MySqlDatabaseProxy implements DatabaseProxyInterface {
    private DatabaseConnectorInterface databaseConnection = null;

    public MySqlDatabaseProxy() {
        this.databaseConnection = new DatabaseConnectorFactory().getDatabaseConnection("MySQL");
    }

    public ArrayList<MessageItem> getInfoList() {
        ArrayList<MessageItem> messageItems = new ArrayList<MessageItem>();
        Connection connection = this.databaseConnection.getConnection();
        String sqlStr = "SELECT * FROM infolist ORDER BY IssueTime DESC;";
        ResultSet resultSet = this.databaseConnection.query(sqlStr);
        try {
            while(resultSet.next()) {
                int ID = Integer.parseInt(resultSet.getString(1));
                boolean infoType = resultSet.getBoolean(2);
                String clientType = resultSet.getString(3);
                String issueTime = resultSet.getString(4);
                String information = resultSet.getString(5);
                String clientIP = resultSet.getString(6);
                String filePath = resultSet.getString(7);
                messageItems.add(new MessageItem(ID, infoType, clientType, issueTime, information, clientIP, filePath));
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
        this.databaseConnection.allClose();
        return messageItems;
    }

    public void insertInfoItem(MessageItem messageItem) {
        Connection connection = this.databaseConnection.getConnection();
        String sqlStr = "INSERT INTO infolist(InfoType, ClientType, IssueTime, Information, ClientIP, FilePath) VALUES("+
                messageItem.getInfoType() +", '"+ messageItem.getClientType() +"', '"+ messageItem.getIssueTime() +"', '"+
                messageItem.getInformation() +"', '"+ messageItem.getClientIP() +"', '"+ messageItem.getFilePath() +"');";
        this.databaseConnection.update(sqlStr);
        this.databaseConnection.allClose();
    }
}
