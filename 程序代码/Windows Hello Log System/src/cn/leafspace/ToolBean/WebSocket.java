package cn.leafspace.ToolBean;

import javax.websocket.*;
import java.io.IOException;
import javax.websocket.server.ServerEndpoint;
import java.util.concurrent.CopyOnWriteArraySet;

/**
 * @ServerEndpoint 注解是一个类层次的注解，它的功能主要是将目前的类定义成一个websocket服务器端,
 * 注解的值将被用于监听用户连接的终端访问URL地址,客户端可以通过这个URL来连接到WebSocket服务器端
 */
@ServerEndpoint("/websocket")
public class WebSocket {
    private static int onlineCount = 0;                                      //记录当前在线连接数
    //concurrent包的线程安全Set，用来存放每个客户端对应的WebSocket对象
    //若要实现服务端与单一客户端通信的话，可以使用Map来存放，其中Key可以为用户标识
    private static CopyOnWriteArraySet<WebSocket> webSocketSet = new CopyOnWriteArraySet<WebSocket>();

    private Session session;                                                 //与某个客户端的连接会话，需要通过它来给客户端发送数据

    public static synchronized int getOnlineCount() {
        return WebSocket.onlineCount;
    }

    public static synchronized void addOnlineCount() {
        WebSocket.onlineCount++;
    }

    public static synchronized void subOnlineCount() {
        WebSocket.onlineCount--;
    }

    @OnOpen
    public void onOpen(Session session) {                                    //连接建立成功调用的方法
        this.session = session;
        WebSocket.webSocketSet.add(this);                                    //存入每个客户端对应的WebSocket对象
        addOnlineCount();                                                    //在线数加1
    }

    @OnClose
    public void onClose() {                                                  //连接关闭调用的方法
        WebSocket.webSocketSet.remove(this);                             //删除客户端对应的WebSocket对象
        subOnlineCount();                                                    //在线数减1
    }

    @OnError
    public void onError(Session session, Throwable error) {                  //发生错误时调用
        error.printStackTrace();
    }

    @OnMessage
    public void onMessage(String message, Session session) {
        for(WebSocket webSocketItem : WebSocket.webSocketSet) {              //群发消息
            try {
                webSocketItem.sendMessage(message);
            } catch (IOException exception) {
                exception.printStackTrace();
            }
        }
    }

    public static void broadcastMessage(String message) {
        for(WebSocket webSocketItem : WebSocket.webSocketSet) {              //群发消息
            try {
                webSocketItem.sendMessage(message);
            } catch (IOException exception) {
                exception.printStackTrace();
            }
        }
    }

    public void sendMessage(String message) throws IOException {
        this.session.getBasicRemote().sendText(message);
    }
}
