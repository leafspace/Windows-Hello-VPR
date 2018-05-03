package cn.leafspace.Thread;

import java.io.*;
import java.net.Socket;

public class ReceiveThread extends Thread {
    private int fileSize;
    private File file;
    private Socket socket;

    public ReceiveThread(int fileSize, File file, Socket socket) {
        this.fileSize = fileSize;
        this.file = file;
        this.socket = socket;
    }

    public void run() {
        try {
            InputStream inputStream = this.socket.getInputStream();
            InputStreamReader inputStreamReader = new InputStreamReader(inputStream, "UTF-8");
            BufferedReader bufferedReader = new BufferedReader(inputStreamReader);
            DataOutputStream dataOutputStream = new DataOutputStream(new FileOutputStream(this.file));

            char[] bufferPool = new char[1024];
            while (this.fileSize != 0) {
                if (this.fileSize >= 1024) {
                    bufferedReader.read(bufferPool);
                    String tempStr = new String(new String(bufferPool).getBytes(), "UTF-8");
                    dataOutputStream.write(tempStr.getBytes(), 0, 1024);
                    this.fileSize -= 1024;
                } else {
                    bufferedReader.read(bufferPool);
                    String tempStr = new String(new String(bufferPool).getBytes(), "UTF-8");
                    dataOutputStream.write(tempStr.getBytes(), 0, this.fileSize);
                    this.fileSize -= this.fileSize;
                }
                System.out.println(bufferPool);
            }
            dataOutputStream.close();
            bufferedReader.close();
            inputStreamReader.close();
            inputStream.close();
            this.socket.close();
            System.out.println("======== 文件接收成功 ========");
        } catch (IOException exception) {
            exception.printStackTrace();
        }
    }

}
