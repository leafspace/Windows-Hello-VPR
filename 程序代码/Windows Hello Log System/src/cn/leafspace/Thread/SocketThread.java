package cn.leafspace.Thread;

import java.net.Socket;
import java.io.IOException;
import java.net.ServerSocket;
import java.io.InputStreamReader;

/**
 * Created by 18852923073 on 2017/3/11.
 */
public class SocketThread extends Thread {
    private final int portNumber = 5000;
    private final int threadNumber = 10;
    private ServerSocket serverSocket;
    private ReceiveTread[] receiveTreads;

    public SocketThread() {
        try {
            this.serverSocket = new ServerSocket(this.portNumber);
            this.receiveTreads = new ReceiveTread[this.threadNumber];
            for(int i = 0; i < this.threadNumber; ++i) {
                this.receiveTreads[i] = null;
            }
        } catch (IOException exception) {
            exception.printStackTrace();
        }
    }

    public void run() {
        try {
            while (!this.isInterrupted() & this.serverSocket != null) {
                for(int i = 0; i < this.threadNumber; ++i) {
                    if(this.receiveTreads[i] == null ||
                            (this.receiveTreads[i] != null && !this.receiveTreads[i].threadState)) {
                        Socket socket = this.serverSocket.accept();
                        InputStreamReader inputStreamReader = new InputStreamReader(socket.getInputStream(), "UTF-8");

                        System.out.println("InetAddress : " + socket.getInetAddress());
                        System.out.println("LocalAddress : " + socket.getLocalAddress());
                        System.out.println("LocalSocketAddress : " + socket.getLocalSocketAddress());
                        System.out.println("RemoteSocketAddress : " + socket.getRemoteSocketAddress() + " 接入系统...");

                        this.receiveTreads[i] = new ReceiveTread(socket, inputStreamReader);
                        this.receiveTreads[i].start();
                    }
                }
            }
        }catch (IOException e){
            e.printStackTrace();
        }
    }

    public void closeServerSocket() {
        try {
            if (this.serverSocket != null && !this.serverSocket.isClosed()) {
                this.serverSocket.close();
            }
        } catch (Exception exception) {
            System.out.println("SocketThread error : " + exception.getMessage());
        } finally {
            this.serverSocket = null;
        }
    }
}