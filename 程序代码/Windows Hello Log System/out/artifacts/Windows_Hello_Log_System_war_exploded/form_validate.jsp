<%@ page import="cn.leafspace.ToolBean.User" %>
<%@ page import="cn.leafspace.ToolBean.MessageItem" %>
<%@ page import="cn.leafspace.Database.Factory.DatabaseProxyFactory" %>
<%@ page import="cn.leafspace.Database.Interface.DatabaseProxyInterface" %>
<%@ page contentType="text/html;charset=UTF-8" language="java" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
    <head>
        <meta charset="utf-8" />
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Windows Hello后台Log系统</title>
        <link rel="stylesheet" href="assets/css/amazeui.css" />
        <link rel="stylesheet" href="assets/css/core.css" />
        <link rel="stylesheet" href="assets/css/menu.css" />
        <link rel="stylesheet" href="assets/css/index.css" />
        <link rel="stylesheet" href="assets/css/admin.css" />
        <link rel="stylesheet" href="assets/css/page/form.css" />
        <link rel="stylesheet" href="assets/css/page/typography.css" />
        <link rel="stylesheet" href="assets/css/font-awesome.min.css">
    </head>
    <%
        User user = (User) request.getSession().getAttribute("user");
        if (user == null) {
            request.getRequestDispatcher("login.jsp?errorInfo=不存在此用户或密码不正确!").forward(request,response);
        }

        int id = 1;
        try {
            id = Integer.parseInt(request.getParameter("ID"));
        } catch (NumberFormatException exception) {
        }

        DatabaseProxyFactory databaseProxyFactory = new DatabaseProxyFactory();
        DatabaseProxyInterface databaseProxyInterface = databaseProxyFactory.getDatabaseProxy("MySQL");
        MessageItem messageItem = databaseProxyInterface.getMessage(id);
        if (messageItem == null) {
            id = 1;
            messageItem = databaseProxyInterface.getMessage(1);
        }

        String information = messageItem.getInformation();
        String result = messageItem.getResult();

        String resultHead = "";
        String resultData = "";
        if (result.split(" ").length > 1) {
            resultHead = result.split(" ")[0];
            resultData = result.split(" ")[1];
        }

        String[] informationList = information.split("\\|");
        String[] resultHeadList = resultHead.split("\\|");
        String[] resultDataList = resultData.split("\\|");
    %>
    <body>
        <header class="am-topbar am-topbar-fixed-top">
            <div class="am-topbar-left am-hide-sm-only">
                <a href="index.jsp" class="logo"><span style="font-family: '微软雅黑';">管理员</span><i class="zmdi zmdi-layers"></i></a>
            </div>

            <div class="contain">
                <ul class="am-nav am-navbar-nav am-navbar-left">
                    <li><h4 class="page-title">日志信息</h4></li>
                </ul>
                <ul class="am-nav am-navbar-nav am-navbar-right">
                    <li class="hidden-xs am-hide-sm-only">
                        <form role="search" class="app-search">
                            <input type="text" placeholder="查找..." class="form-control">
                            <a href=""><img src="assets/img/search.png"></a>
                        </form>
                    </li>
                </ul>
            </div>
        </header>

        <div class="admin">
            <div class="admin-sidebar am-offcanvas  am-padding-0" id="admin-offcanvas">
                <div class="am-offcanvas-bar admin-offcanvas-bar">
                    <div class="user-box am-hide-sm-only">
                        <div class="user-img">
                            <img src="assets/img/avatar-1.jpg" alt="user-img" class="img-circle img-thumbnail img-responsive">
                            <div class="user-status offline"><i class="am-icon-dot-circle-o" aria-hidden="true"></i></div>
                        </div>
                        <h5><a href="#"><%=user.getUsername()%></a> </h5>
                        <ul class="list-inline">
                            <li>
                                <a href="#">
                                    <i class="am-icon-cog" aria-hidden="true"></i>
                                </a>
                            </li>

                            <li>
                                <a href="#" class="text-custom">
                                    <i class="am-icon-cog" aria-hidden="true"></i>
                                </a>
                            </li>
                        </ul>
                    </div>
                    <ul class="am-list admin-sidebar-list">
                        <li><a href="index.jsp"><span class="am-icon-home"></span> 首页</a></li>
                        <li class="admin-parent">
                            <a class="am-cf" data-am-collapse="{target: '#collapse-nav1'}">
                                <span class="am-icon-table"></span> 表格 <span class="am-icon-angle-right am-fr am-margin-right"></span>
                            </a>
                            <ul class="am-list admin-sidebar-sub am-collapse" id="collapse-nav1">
                                <li><a href="table_complete.jsp">完整表格</a></li>
                            </ul>
                        </li>
                    </ul>
                </div>
            </div>
            <div class="content-page">
                <div class="content">
                    <div class="am-g">
                        <div class="am-u-sm-12">
                            <div class="card-box">
                                <ul class="am-nav am-fr">
                                    <li class="am-dropdown" onclick="jumpBack()">
                                        <a class="am-dropdown-toggle">
                                            <span class="am-icon-caret-left">&nbsp;返回</span>
                                        </a>
                                    </li>
                                </ul>

                                <form action="" class="am-form" data-am-validator>
                                    <fieldset>
                                        <legend>数据 （ID:<%=id%>）</legend>

                                        <div class="am-form-group">
                                            <div class="am-form-group" style="width: 48%; float: left;">
                                                <label>状态：</label>
                                                <select id="InfoType">
                                                    <option value="1" <%if(messageItem.getInfoType()){%>selected<%}%>>成功</option>
                                                    <option value="0" <%if(!messageItem.getInfoType()){%>selected<%}%>>失败</option>
                                                </select>
                                            </div>
                                            <div class="am-form-group" style="width: 48%; float: right;">
                                                <label>客户端：</label>
                                                <select id="ClientType">
                                                    <option value="Windows Hello" <%if(messageItem.getClientType().equals("Windows Hello")){%>selected<%}%>>
                                                        Windows Hello
                                                    </option>
                                                    <option value="Windows Hello Client" <%if(!messageItem.getClientType().equals("Windows Hello")){%>selected<%}%>>
                                                        Windows Hello Client
                                                    </option>
                                                </select>
                                            </div>
                                        </div>

                                        <div class="am-form-group">
                                            <div class="am-form-group" style="width: 48%; float: left;">
                                                <label>发布时间：</label>
                                                <input type="text" minlength="3" placeholder="输入时间（格式：年-月-日 时:分:秒）" value="<%=messageItem.getIssueTime()%>" required/>
                                            </div>
                                            <div class="am-form-group" style="width: 48%; float: right;">
                                                <label>客户端IP：</label>
                                                <input type="text" minlength="3" placeholder="输入IP（格式：IP:PORT）" value="<%=messageItem.getClientIP()%>" required/>
                                            </div>
                                        </div>

                                        <div class="am-form-group">
                                            <label style="width: 100%">识别数据：</label>
                                            <%
                                                int tableNumber = 0;
                                                int tableSize = 10;
                                                if (resultHeadList.length % tableSize != 0) {
                                                    tableNumber = resultHeadList.length / tableSize + 1;
                                                } else {
                                                    tableNumber = resultHeadList.length / tableSize;
                                                }
                                                for (int i = 0; i < tableNumber; ++i) {
                                            %>
                                                    <table class="am-table  am-table-striped">
                                                        <thead>
                                                            <tr>
                                                                <%
                                                                    for (int j = i * tableSize; j < Math.min((i + 1) * tableSize, resultHeadList.length); ++j) {
                                                                        String headName = "";
                                                                        if (resultHeadList[j].indexOf(".txt") >= 0) {
                                                                            headName = resultHeadList[j].substring(0, resultHeadList[j].indexOf(".txt"));
                                                                        } else {
                                                                            headName = resultHeadList[j];
                                                                        }

                                                                        if (headName.indexOf("-") < headName.indexOf("(")) {
                                                                            headName = headName.substring(headName.indexOf("-") + 1, headName.indexOf("("));
                                                                        }
                                                                %>
                                                                        <th><%=headName%></th>
                                                                <%
                                                                    }
                                                                %>
                                                            </tr>
                                                        </thead>
                                                        <tbody>
                                                            <tr>
                                                                <%
                                                                    for (int j = i * tableSize; j < Math.min((i + 1) * tableSize, resultDataList.length); ++j) {
                                                                %>
                                                                        <td><%=resultDataList[j]%></td>
                                                                <%
                                                                    }
                                                                %>
                                                            </tr>
                                                        </tbody>
                                                    </table>
                                            <%
                                                }
                                            %>
                                        </div>

                                        <div class="am-form-group">
                                            <label style="width: 100%">记录语音：</label>
                                            <audio controls="controls" height="100" width="50">
                                                <source src="wavLib/<%=messageItem.getFilePath()%>" type="audio/wav" />
                                            </audio>
                                        </div>

                                        <div class="am-form-group">
                                            <label for="doc-vld-ta-2">日志信息：</label>
                                            <textarea rows="15" id="doc-vld-ta-2" minlength="10" readonly><%for (int i = 0; i < informationList.length; ++i) {%><%=informationList[i] + "\n"%><%}%></textarea>
                                        </div>

                                        <button class="am-btn am-btn-secondary" type="submit">提交</button>
                                    </fieldset>
                                </form>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>

        <a href="admin-offcanvas" class="am-icon-btn am-icon-th-list am-show-sm-only admin-menu" data-am-offcanvas="{target: '#admin-offcanvas'}"></a>
        <script language='javascript' type='text/javascript'>
            function jumpBack() {
                window.location.href="table_complete.jsp";
                //window.history.back(-1);
            }
        </script>
        <script type="text/javascript" src="assets/js/jquery-2.1.0.js" ></script>
        <script type="text/javascript" src="assets/js/amazeui.min.js"></script>
        <script type="text/javascript" src="assets/js/app.js" ></script>
        <script type="text/javascript" src="assets/js/blockUI.js" ></script>
    </body>
</html>
