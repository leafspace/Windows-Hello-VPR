package cn.leafspace.Database.Interface;

import cn.leafspace.ToolBean.MessageItem;

import java.util.ArrayList;

public interface DatabaseProxyInterface {
    int findUser(String username, String password);
    ArrayList<MessageItem> getInfoList();
    ArrayList<MessageItem> getKeywordInfoList(String keyWord);
    MessageItem getMessage(int id);
    void insertInfoItem(MessageItem messageItem);
}
