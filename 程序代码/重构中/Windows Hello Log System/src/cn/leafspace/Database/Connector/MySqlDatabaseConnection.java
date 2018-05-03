package cn.leafspace.Database.Connector;

import cn.leafspace.Database.Interface.DatabaseConnectorInterface;
import cn.leafspace.ToolBean.JSON.JSONObject;
import org.apache.commons.io.FileUtils;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.File;

import java.net.URL;
import java.sql.*;

public class MySqlDatabaseConnection implements DatabaseConnectorInterface {
    private static boolean isInitFlag = false;
    private static String driverName = "com.mysql.jdbc.Driver";
    private static String userName = "root";
    private static String userPassword = "123456";
    private static String databaseIp = "127.0.0.1";
    private static String dbName = "windowshello-vpr-logsystem";

    private Connection connection;
    private PreparedStatement preparedStatement;

    public MySqlDatabaseConnection() {
        try {
            if (!MySqlDatabaseConnection.isInitFlag) {
                URL url = Thread.currentThread().getContextClassLoader().getResource("");
                String configurePath = url.getPath().replace("classes/", "info.json").substring(1);
                configurePath = configurePath.replaceAll("/", "\\\\\\\\");

                File file = new File(configurePath);
                String content = FileUtils.readFileToString(file, "UTF-8");
                JSONObject jsonObject = new JSONObject(content);
                MySqlDatabaseConnection.dbName = jsonObject.getString("dbName");
                MySqlDatabaseConnection.userName = jsonObject.getString("userName");
                MySqlDatabaseConnection.userPassword = jsonObject.getString("userPassword");
                MySqlDatabaseConnection.databaseIp = jsonObject.getString("dbHost");
                MySqlDatabaseConnection.isInitFlag = true;
            }
        } catch (FileNotFoundException e) {
            System.out.println("Error : Can't find profile !");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public Connection getConnection() {
        try {
            String url = "jdbc:mysql://" + MySqlDatabaseConnection.databaseIp + ":3306/" + MySqlDatabaseConnection.dbName +
                    "?useUnicode=true&characterEncoding=utf8&user=" + MySqlDatabaseConnection.userName + "&password=" + MySqlDatabaseConnection.userPassword +
                    "&useSSL=false";
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
