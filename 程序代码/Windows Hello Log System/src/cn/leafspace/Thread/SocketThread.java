package cn.leafspace.Thread;

import cn.leafspace.Database.Interface.DatabaseProxyInterface;
import cn.leafspace.Database.Factory.DatabaseProxyFactory;
import cn.leafspace.ToolBean.MessageItem;

import java.io.*;
import java.util.Date;
import java.net.Socket;
import java.net.ServerSocket;
import java.text.SimpleDateFormat;

/**
 * Created by 18852923073 on 2017/3/11.
 */
public class SocketThread extends Thread {
    private final int portNumber = 5000;
    private final int threadNumber = 1;
    private ServerSocket serverSocket;

    public SocketThread() {
        try {
            this.serverSocket = new ServerSocket(this.portNumber);
        } catch (IOException exception) {
            exception.printStackTrace();
        }
    }

    public void run() {
        try {
            while (!this.isInterrupted() & this.serverSocket != null) {
                for(int i = 0; i < this.threadNumber; ++i) {
                    Socket socket = this.serverSocket.accept();
                    InputStreamReader inputStreamReader = new InputStreamReader(socket.getInputStream(), "UTF-8");

                    System.out.println("InetAddress : " + socket.getInetAddress());
                    System.out.println("LocalAddress : " + socket.getLocalAddress());
                    System.out.println("LocalSocketAddress : " + socket.getLocalSocketAddress());
                    System.out.println("RemoteSocketAddress : " + socket.getRemoteSocketAddress() + " 接入系统...");
                    
                    SimpleDateFormat dataFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
                    boolean infoType = true;
                    String clientType = "Windows Hello";
                    String issueTime = dataFormat.format(new Date());
                    String information = "";
                    String clientIP = socket.getRemoteSocketAddress().toString().substring(1);
                    String filePath = "";

                    BufferedReader bufferedReader = new BufferedReader(inputStreamReader);
                    while (true) {
                        String targetFlag = bufferedReader.readLine();
                        System.out.println(targetFlag);
                        if (targetFlag.equals("<Message>")) {
                            String tempStr = bufferedReader.readLine();
                                if (tempStr.indexOf("ERROR") >= 0) {
                                    infoType = false;
                                }
                            information = information + tempStr + "\n";
                        } else if (targetFlag.equals("<type>")) {
                            clientType = bufferedReader.readLine();
                        } else if (targetFlag.equals("<File>")) {
                            SimpleDateFormat tempDataFormat = new SimpleDateFormat("yyyyMMddHHmmss");
                            String tempNowTime = tempDataFormat.format(new Date());

                            filePath = "wavLib//" + tempNowTime + ".wav";
                            char[] bufferPool = new char[1024];
                            File file = new File(filePath);
                            if (!file.exists()) {
                                file.createNewFile();
                            }

                            FileWriter fileWriter = new FileWriter(file.getName(), true);
                            BufferedWriter bufferedWriter = new BufferedWriter(fileWriter);
                            while (inputStreamReader.read(bufferPool) != -1) {
                                bufferedWriter.write(bufferPool);
                            }
                            bufferedWriter.close();
                            delete bufferPool;
                        } else if (targetFlag.equals("<Finish>")) {
                            break;
                        }
                    }

                    MessageItem messageItem = new MessageItem(infoType, clientType, issueTime, information, clientIP, filePath);
                    DatabaseProxyFactory databaseProxyFactory = new DatabaseProxyFactory();
                    DatabaseProxyInterface databaseProxyInterface = databaseProxyFactory.getDatabaseProxy("MySQL");
                    databaseProxyInterface.insertInfoItem(messageItem);
                }
            }
        } catch (IOException exception){
            exception.printStackTrace();
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