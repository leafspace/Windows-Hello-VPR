package cn.leafspace.Thread;

import java.io.*;
import java.util.Date;
import java.net.Socket;
import java.text.SimpleDateFormat;

public class ReceiveTread extends Thread {
    private Socket socket;
    private InputStreamReader inputStreamReader;
    private boolean infoType = true;
    public boolean threadState = true;

    public ReceiveTread(Socket socket, InputStreamReader inputStreamReader) {
        this.socket = socket;
        this.inputStreamReader = inputStreamReader;
    }

    public void run() {
        String information = "";
        SimpleDateFormat dataFormat = new SimpleDateFormat("yyyyMMddHHmmss");
        String nowTime = dataFormat.format(new Date());
        String filePath = "";
        try {
            BufferedReader bufferedReader = new BufferedReader(this.inputStreamReader);
            while (true) {
                String targetFlag = bufferedReader.readLine();
                if (targetFlag.equals("<Message>")) {
                    String tempStr = bufferedReader.readLine();
                    if (this.threadState) {
                        if (tempStr.indexOf("ERROR") >= 0) {
                            this.threadState = false;
                        }
                    }
                    information = information + tempStr + "\n";
                } else if (targetFlag.equals("<File>")) {
                    filePath = "wavLib//" + nowTime + ".wav";
                    char[] bufferPool = new char[1024];
                    File file = new File(filePath);
                    if (!file.exists()) {
                        file.createNewFile();
                    }

                    FileWriter fileWriter = new FileWriter(file.getName(), true);
                    BufferedWriter bufferedWriter = new BufferedWriter(fileWriter);
                    while (this.inputStreamReader.read(bufferPool) != -1) {
                        bufferedWriter.write(bufferPool);
                    }
                    bufferedWriter.close();
                } else if (targetFlag.equals("<Finish>")) {
                    break;
                }
            }
        } catch (IOException exception) {
            exception.printStackTrace();
        }

        //Todo Write to database

        this.threadState = false;
    }


}
