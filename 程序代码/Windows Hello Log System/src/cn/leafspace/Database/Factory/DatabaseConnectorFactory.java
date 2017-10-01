package cn.leafspace.Database.Factory;

import cn.leafspace.Database.Connector.MySqlDatabaseConnection;
import cn.leafspace.Database.Interface.DatabaseConnectorInterface;

public class DatabaseConnectorFactory {
    private DatabaseConnectorInterface databaseConnection;

    public DatabaseConnectorFactory() {
        this.databaseConnection = null;
    }

    public DatabaseConnectorInterface getDatabaseConnection(String flag) {
        if(flag.equals("MySQL")) {
            this.databaseConnection = new MySqlDatabaseConnection();
        } else if(flag.equals("SQL Server")){
            this.databaseConnection = new MySqlDatabaseConnection();                               //在此处暂时放置出品MYSQL CONNECTION
        } else {
            this.databaseConnection = new MySqlDatabaseConnection();                               //在此处暂时放置出品MYSQL CONNECTION
        }
        return this.databaseConnection;
    }
}
