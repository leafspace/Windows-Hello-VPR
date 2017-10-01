package cn.leafspace.Database.Interface;

import cn.leafspace.ToolBean.MessageItem;

import java.util.ArrayList;

public interface DatabaseProxyInterface {
    ArrayList<MessageItem> getInfoList();
    void insertInfoItem(MessageItem messageItem);
}
