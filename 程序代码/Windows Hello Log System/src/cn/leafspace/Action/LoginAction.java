package cn.leafspace.Action;

import cn.leafspace.ToolBean.User;
import cn.leafspace.ActionForm.LoginActionForm;
import cn.leafspace.Database.Factory.DatabaseProxyFactory;
import cn.leafspace.Database.Interface.DatabaseProxyInterface;

import org.apache.struts.action.Action;
import org.apache.struts.action.ActionForm;
import org.apache.struts.action.ActionForward;
import org.apache.struts.action.ActionMapping;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

public class LoginAction extends Action {
    @Override
    public ActionForward execute(ActionMapping mapping, ActionForm form,
                                 HttpServletRequest request, HttpServletResponse response)
            throws Exception {
        LoginActionForm loginActionForm = (LoginActionForm) form;
        DatabaseProxyFactory databaseProxyFactory = new DatabaseProxyFactory();
        DatabaseProxyInterface databaseProxyInterface = databaseProxyFactory.getDatabaseProxy("MySQL");
        int userID = databaseProxyInterface.findUser(loginActionForm.getUsername(), loginActionForm.getPassword());
        if (userID > 0) {
            User user = new User(userID, loginActionForm.getUsername(), loginActionForm.getPassword());
            request.getSession().setAttribute("user", user);
            return mapping.findForward("userPage");
        } else {
            request.getSession().setAttribute("errorInfo", "不存在此用户或密码不正确!");
        }
        return mapping.findForward("error");
    }
}
