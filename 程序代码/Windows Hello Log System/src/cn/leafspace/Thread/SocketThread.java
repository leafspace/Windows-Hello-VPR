package cn.leafspace.Thread;

import cn.leafspace.Database.Interface.DatabaseProxyInterface;
import cn.leafspace.Database.Factory.DatabaseProxyFactory;
import cn.leafspace.ToolBean.MessageItem;

import java.io.*;
import java.util.Date;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.net.ServerSocket;
import java.nio.charset.Charset;
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
                    InputStream inputStream = socket.getInputStream();
                    InputStreamReader inputStreamReader = new InputStreamReader(inputStream);
                    BufferedReader bufferedReader = new BufferedReader(inputStreamReader);

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

                    while (true) {
                        String targetFlag = bufferedReader.readLine();
                        if (targetFlag == null) {
                            break;
                        }
                        if (targetFlag.equals("<Message>")) {
                            String tempStr = new String(bufferedReader.readLine().getBytes("GBK"), "UTF-8");
                            if (tempStr.indexOf("ERROR") >= 0) {
                                infoType = false;
                            }
                            information = information + tempStr + "\n";
                        } else if (targetFlag.equals("<type>")) {
                            clientType = new String(bufferedReader.readLine().getBytes("GBK"), "UTF-8");
                        } else if (targetFlag.equals("<File>")) {
                            SimpleDateFormat tempDataFormat = new SimpleDateFormat("yyyyMMddHHmmss");
                            String tempNowTime = tempDataFormat.format(new Date());

                            filePath = ".//wavLib//" + tempNowTime + ".wav";
                            File file = new File(filePath);
                            if (!file.exists()) {
                                file.getParentFile().mkdirs();
                                file.createNewFile();
                            }

                            int fileSize = -1;
                            try {
                                fileSize = Integer.parseInt(bufferedReader.readLine());
                            } catch (NumberFormatException exception) {
                                exception.printStackTrace();
                            }

                            ReceiveThread receiveThread = new ReceiveThread(fileSize, file, socket);
                            receiveThread.start();
                            break;
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

    private byte[] getBytes(char[] charStr) {
        Charset charset = Charset.forName ("UTF-8");
        CharBuffer charBuffer = CharBuffer.allocate(charStr.length);
        charBuffer.flip ();
        ByteBuffer byteBuffer = charset.encode(charBuffer);
        return byteBuffer.array();
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