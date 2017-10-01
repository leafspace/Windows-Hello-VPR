package cn.leafspace.Database.Interface;

import java.sql.ResultSet;
import java.sql.Connection;

public interface DatabaseConnectorInterface {
    Connection getConnection();
    void allClose();
    void update(String sql);
    ResultSet query(String sql);
}
