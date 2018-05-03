package cn.leafspace.ActionForm;

import org.apache.struts.action.ActionForm;
import org.apache.struts.action.ActionErrors;
import javax.servlet.http.HttpServletRequest;
import org.apache.struts.action.ActionMapping;

public class LoginActionForm extends ActionForm {
    private String username;
    private String password;

    public String getUsername() {
        return this.username;
    }

    public String getPassword() {
        return this.password;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public ActionErrors validate(ActionMapping actionMapping, HttpServletRequest request) {
        if(this.username == null) {
            this.username = "";
        }

        if(this.password == null) {
            this.password = "";
        }

        this.username.trim();
        this.password.trim();

        return null;
    }
}
