package cn.leafspace.Database.Factory;

import cn.leafspace.Database.Proxy.MySqlDatabaseProxy;
import cn.leafspace.Database.Interface.DatabaseProxyInterface;

public class DatabaseProxyFactory {
    private DatabaseProxyInterface databaseProxy;

    public DatabaseProxyFactory() {
        this.databaseProxy = null;
    }

    public DatabaseProxyInterface getDatabaseProxy(String flag) {
        if(flag.equals("MySQL")) {
            this.databaseProxy = new MySqlDatabaseProxy();
        } else if(flag.equals("SQL Server")){
            this.databaseProxy = new MySqlDatabaseProxy();                     //在此处暂时放置出品MYSQL Proxy
        } else {
            this.databaseProxy = new MySqlDatabaseProxy();                     //在此处暂时放置出品MYSQL Proxy
        }
        return this.databaseProxy;
    }
}
