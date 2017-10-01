package cn.leafspace.ToolBean;

public class MessageItem {
    private int ID;
    private boolean infoType;
    private String issueTime;
    private String information;
    private String filePath;

    public MessageItem(boolean infoType, String issueTime, String information, String filePath) {
        this.infoType = infoType;
        this.issueTime = issueTime;
        this.information = information;
        this.filePath = filePath;
    }

    public MessageItem(int ID, boolean infoType, String issueTime, String information, String filePath) {
        this(infoType, issueTime, information, filePath);
        this.ID = ID;
    }

    public int getID() {
        return this.ID;
    }

    public boolean getInfoType() {
        return this.infoType;
    }

    public String getIssueTime() {
        return this.issueTime;
    }

    public String getInformation() {
        return this.information;
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

    public void setIssueTime(String issueTime) {
        this.issueTime = issueTime;
    }

    public void setInformation(String information) {
        this.information = information;
    }

    public void setFilePath(String filePath) {
        this.filePath = filePath;
    }
}
