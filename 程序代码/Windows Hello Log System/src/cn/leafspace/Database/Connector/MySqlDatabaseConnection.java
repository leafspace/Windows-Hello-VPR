package cn.leafspace.Database.Connector;

import cn.leafspace.Database.Interface.DatabaseConnectorInterface;

import java.sql.*;

public class MySqlDatabaseConnection implements DatabaseConnectorInterface {
    private static String driverName = "com.mysql.jdbc.Driver";
    private static String userName = "root";
    private static String userPassword = "123456";
    private static String databaseIp = "192.168.155.3";
    private static String dbName = "windowshello-vpr-logsystem";

    private Connection connection;
    private PreparedStatement preparedStatement;

    public Connection getConnection() {
        try {
            String url = "jdbc:mysql://" + MySqlDatabaseConnection.databaseIp + ":3306/" + MySqlDatabaseConnection.dbName +
                    "?useUnicode=true&characterEncoding=utf8&user=" + MySqlDatabaseConnection.userName + "&password=" + MySqlDatabaseConnection.userPassword;
            Class.forName(MySqlDatabaseConnection.driverName);               //加载数据库驱动程序类
            this.connection = DriverManager.getConnection(url);              //获取数据库链接
            return this.connection;
        } catch (ClassNotFoundException e) {
            System.out.println("Error : No Mysql driver !");
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return null;
    }

    public void allClose() {
        try{
            if(this.preparedStatement != null){
                this.preparedStatement.close();
            }
            if(this.connection != null){
                this.connection.close();
            }
        }catch (SQLException e){
            e.printStackTrace();
        }
    }

    public void update(String sql) {
        try {
            this.preparedStatement = this.connection.prepareStatement(sql);
            this.preparedStatement.executeUpdate();
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }

    public ResultSet query(String sql) {
        try {
            this.preparedStatement = this.connection.prepareStatement(sql);
            ResultSet resultSet = this.preparedStatement.executeQuery();
            return resultSet;
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return null;
    }
}
