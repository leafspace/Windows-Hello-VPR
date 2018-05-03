package cn.leafspace.ToolBean;

public class MessageItem {
    private int ID;
    private boolean infoType;
    private String clientType;
    private String issueTime;
    private String information;
    private String result;
    private String clientIP;
    private String filePath;

    public MessageItem(boolean infoType, String clientType, String issueTime, String information, String filePath) {
        this.infoType = infoType;
        this.clientType = clientType;
        this.issueTime = issueTime;
        this.information = information;
        this.filePath = filePath;
    }

    public MessageItem(boolean infoType, String clientType, String issueTime, String information, String clientIP, String filePath) {
        this(infoType, clientType, issueTime, information, filePath);
        this.clientIP = clientIP;
    }

    public MessageItem(boolean infoType, String clientType, String issueTime, String information, String result, String clientIP, String filePath) {
        this(infoType, clientType, issueTime, information, clientIP, filePath);
        this.result = result;
    }

    public MessageItem(int ID, boolean infoType, String clientType, String issueTime, String information, String filePath) {
        this(infoType, clientType, issueTime, information, filePath);
        this.ID = ID;
    }

    public MessageItem(int ID, boolean infoType, String clientType, String issueTime, String information, String result, String clientIP, String filePath) {
        this(ID, infoType, clientType, issueTime, information, filePath);
        this.result = result;
        this.clientIP = clientIP;
    }

    public int getID() {
        return this.ID;
    }

    public boolean getInfoType() {
        return this.infoType;
    }

    public String getClientType() {
        return this.clientType;
    }

    public String getIssueTime() {
        return this.issueTime;
    }

    public String getInformation() {
        return this.information;
    }

    public String getResult() {
        return this.result;
    }

    public String getClientIP() {
        return this.clientIP;
    }

    public String getFilePath() {
        return this.filePath;
    }

    public void setID(int ID) {
        this.ID = ID;
    }

    public void setInfoType(boolean infoType) {
        this.infoType = infoType;
    }

    public void setClientType(String clientType) {
        this.clientType = clientType;
    }

    public void setIssueTime(String issueTime) {
        this.issueTime = issueTime;
    }

    public void setInformation(String information) {
        this.information = information;
    }

    public void setResult(String result) {
        this.result = result;
    }

    public void setClientIP(String clientIP) {
        this.clientIP = clientIP;
    }

    public void setFilePath(String filePath) {
        this.filePath = filePath;
    }
}
