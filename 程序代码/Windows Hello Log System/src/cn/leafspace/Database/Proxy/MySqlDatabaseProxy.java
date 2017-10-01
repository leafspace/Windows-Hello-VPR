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
                boolean infoType = Boolean.parseBoolean(resultSet.getString(2));
                String issueTime = resultSet.getString(3);
                String information = resultSet.getString(4);
                String filePath = resultSet.getString(5);
                messageItems.add(new MessageItem(ID, infoType, issueTime, information, filePath));
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
        this.databaseConnection.allClose();
        return messageItems;
    }

    public void insertInfoItem(MessageItem messageItem) {
        Connection connection = this.databaseConnection.getConnection();
        String sqlStr = "INSERT INTO infolist(InfoType, IssueTime, Information, FilePath) VALUES("+
                messageItem.getInfoType() +", '"+ messageItem.getIssueTime() +"', '"+ messageItem.getInformation() +"', '"+
                messageItem.getFilePath() +"');";
        this.databaseConnection.update(sqlStr);
        this.databaseConnection.allClose();
    }
}
