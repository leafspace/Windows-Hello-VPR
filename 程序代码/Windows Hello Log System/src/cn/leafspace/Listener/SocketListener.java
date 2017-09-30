package cn.leafspace.Listener;

import cn.leafspace.Thread.SocketThread;
import javax.servlet.http.HttpSessionEvent;
import javax.servlet.http.HttpSessionListener;

public class SocketListener implements HttpSessionListener {
    private SocketThread socketThread;

    public void sessionCreated (HttpSessionEvent sessionEvent) {
        System.out.println("Server contextInitialized over!");
        if (this.socketThread == null && SocketThread.userNumber > 0)
        {
            this.socketThread = new SocketThread();
            this.socketThread.start();
        }
    }

    public void sessionDestroyed (HttpSessionEvent sessionEvent) {
        if (this.socketThread != null && this.socketThread.isInterrupted())
        {
            this.socketThread.closeServerSocket();
            this.socketThread.interrupt();
        }
    }
}
