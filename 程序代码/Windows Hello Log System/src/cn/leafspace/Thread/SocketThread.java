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
                SimpleDateFormat dataFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
                boolean infoType = true;
                String clientType = "Windows Hello";
                String issueTime = dataFormat.format(new Date());
                String information = "";
                String result = "";
                String clientIP = "Unknow";
                String filePath = "";
                try {
                    for(int i = 0; i < this.threadNumber; ++i) {
                        Socket socket = this.serverSocket.accept();
                        InputStream inputStream = socket.getInputStream();
                        InputStreamReader inputStreamReader = new InputStreamReader(inputStream);
                        BufferedReader bufferedReader = new BufferedReader(inputStreamReader);

                        System.out.println("InetAddress : " + socket.getInetAddress());
                        System.out.println("LocalAddress : " + socket.getLocalAddress());
                        System.out.println("LocalSocketAddress : " + socket.getLocalSocketAddress());
                        System.out.println("RemoteSocketAddress : " + socket.getRemoteSocketAddress() + " 接入系统...");

                        dataFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
                        issueTime = dataFormat.format(new Date());
                        clientIP = socket.getRemoteSocketAddress().toString().substring(1);

                        while (true) {
                            String targetFlag = bufferedReader.readLine();
                            if (targetFlag == null) {
                                break;
                            }
                            if (targetFlag.equals("<Message>")) {
                                //String tempStr = new String(bufferedReader.readLine().getBytes("GBK"), "UTF-8");
                                String tempStr = bufferedReader.readLine();
                                if (tempStr.indexOf("ERROR") >= 0) {
                                    infoType = false;
                                }
                                information = information + tempStr + "|";
                            } else if (targetFlag.equals("<Type>")) {
                                clientType = bufferedReader.readLine();
                            } else if (targetFlag.equals("<File>")) {
                                filePath = bufferedReader.readLine();
                                int lastIndex = filePath.lastIndexOf('\\');
                                filePath = filePath.substring(lastIndex + 1);
                            /*
                            //Socket 接受文件方法（未成功）
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

                            //ReceiveThread receiveThread = new ReceiveThread(fileSize, file, socket);
                            //receiveThread.start();

                            ChangeCharset changeCharset = new ChangeCharset();
                            DataOutputStream dataOutputStream = new DataOutputStream(new FileOutputStream(file));

                            char[] bufferPool = new char[1024];
                            while (fileSize != 0) {
                                int readSize = bufferedReader.read(bufferPool);
                                String tempStr = bufferPool.toString();
                                if (fileSize >= 1024) {
                                    dataOutputStream.write(tempStr.getBytes());
                                    fileSize -= 1024;
                                } else {
                                    dataOutputStream.write(tempStr.getBytes());
                                    fileSize -= fileSize;
                                }
                                System.out.println(bufferPool);
                            }
                            dataOutputStream.close();
                            bufferedReader.close();
                            inputStreamReader.close();
                            inputStream.close();
                            socket.close();
                            System.out.println("======== 文件接收成功 ========");
                            */
                                break;
                            } else if (targetFlag.equals("<Result>")) {
                                String tableHead = bufferedReader.readLine();
                                String tableBody = bufferedReader.readLine();
                                result = tableHead + " " + tableBody;
                            } else if (targetFlag.equals("<Finish>")) {
                                break;
                            }
                        }

                        System.out.println(information);
                        System.out.println(filePath);
                        bufferedReader.close();
                        inputStreamReader.close();
                        inputStream.close();
                        socket.close();
                    }
                } catch (IOException exception) {
                    System.out.println("Connection reset.");
                } finally {
                    if (clientType.equals("Windows Hello") && result.length() == 0) {
                        infoType = false;
                    }
                    MessageItem messageItem = new MessageItem(infoType, clientType, issueTime, information, result, clientIP, filePath);
                    DatabaseProxyFactory databaseProxyFactory = new DatabaseProxyFactory();
                    DatabaseProxyInterface databaseProxyInterface = databaseProxyFactory.getDatabaseProxy("MySQL");
                    databaseProxyInterface.insertInfoItem(messageItem);
                }
            }
        } catch (Exception exception){
            exception.printStackTrace();
        } finally {

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