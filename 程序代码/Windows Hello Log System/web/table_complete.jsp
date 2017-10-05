<%@ page import="java.util.ArrayList" %>
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
		<link rel="stylesheet" href="assets/css/font-awesome.min.css">
		<link rel="stylesheet" href="assets/css/page/typography.css" />
	</head>

    <%
        DatabaseProxyFactory databaseProxyFactory = new DatabaseProxyFactory();
        DatabaseProxyInterface databaseProxyInterface = databaseProxyFactory.getDatabaseProxy("MySQL");
        ArrayList<MessageItem> messageItems = databaseProxyInterface.getInfoList();

        int index = 1;
        try {
            index = Integer.parseInt(request.getParameter("index"));
        } catch (NumberFormatException exception) {
        }
        final int pageSize = 15;
        int pageLength = 1;
        if (messageItems.size() % pageSize != 0) {
            pageLength = messageItems.size() / pageSize + 1;
        } else {
            pageLength = messageItems.size() / pageSize;
        }
    %>

	<body>
		<header class="am-topbar am-topbar-fixed-top">		
			<div class="am-topbar-left am-hide-sm-only">
				<a href="index.jsp" class="logo"><span style="font-family: '微软雅黑';">管理员</span><i class="zmdi zmdi-layers"></i></a>
			</div>

			<div class="contain">
				<ul class="am-nav am-navbar-nav am-navbar-left">
					<li><h4 class="page-title">日志列表</h4></li>
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
                            <img src="assets/img/avatar-1.jpg" alt="user-img" title="leafspace" class="img-circle img-thumbnail img-responsive">
                            <div class="user-status offline"><i class="am-icon-dot-circle-o" aria-hidden="true"></i></div>
                        </div>
                        <h5><a href="#">leafspace</a> </h5>
                        <ul class="list-inline">
                            <li>
                                <a href="#">
                                    <i class="fa fa-cog" aria-hidden="true"></i>
                                </a>
                            </li>

                            <li>
                                <a href="#" class="text-custom">
                                    <i class="fa fa-cog" aria-hidden="true"></i>
                                </a>
                            </li>
                        </ul>
                    </div>

                    <ul class="am-list admin-sidebar-list">
                        <li><a href="index.jsp"><span class="am-icon-home"></span> 首页</a></li>
                        <li class="admin-parent">
                            <a class="am-cf am-collapsed" data-am-collapse="{target: '#collapse-nav1'}">
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
                    <div class="card-box">
                        <div class="am-g">
                            <div class="am-u-sm-12 am-u-md-6">
                                <div class="am-btn-toolbar">
                                    <div class="am-btn-group am-btn-group-xs">
                                        <button type="button" class="am-btn am-btn-default"><span class="am-icon-plus"></span> 新增</button>
                                        <button type="button" class="am-btn am-btn-default"><span class="am-icon-save"></span> 保存</button>
                                        <button type="button" class="am-btn am-btn-default"><span class="am-icon-archive"></span> 审核</button>
                                        <button type="button" class="am-btn am-btn-default"><span class="am-icon-trash-o"></span> 删除</button>
                                    </div>
                                </div>
                            </div>

                            <div class="am-u-sm-12 am-u-md-3">
                                <form name="search" method="post" action="">
                                    <div class="am-input-group am-input-group-sm">
                                        <input type="text" class="am-form-field">
                                        <span class="am-input-group-btn">
                                            <button class="am-btn am-btn-default" type="submit">搜索</button>
                                        </span>
                                    </div>
                                </form>
                            </div>
                        </div>

                        <div class="am-g">
                            <div class="am-u-sm-12">
                                <form class="am-form">
                                    <table class="am-table am-table-striped am-table-hover table-main">
                                        <thead>
                                            <tr>
                                                <th class="table-check" style="width: 2%;"><input type="checkbox" name="idList" onclick="swapCheck()"/></th>
                                                <th class="table-id" style="width: 2%;">ID</th>
                                                <th class="table-title" style="width: 18%;">客户端类型</th>
                                                <th class="table-type" style="width: 7%;">状态</th>
                                                <th class="table-author am-hide-sm-only" style="width: 10%;">客户端IP</th>
                                                <th class="table-date am-hide-sm-only" style="width: 18%;">发布时间</th>
                                                <th class="table-set" style="width: 47%;">操作</th>
                                            </tr>
                                        </thead>
                                        <tbody>
                                            <%
                                                for (int i = 0; i < messageItems.size(); ++i) {
                                            %>
                                                    <tr>
                                                        <td><input type="checkbox" name="idList" value="<%=messageItems.get(i).getID()%>"/></td>
                                                        <td><%=messageItems.get(i).getID()%></td>
                                                        <td><a href="#"><%=messageItems.get(i).getClientType()%></a></td>
                                                        <td>
                                                            <%
                                                                if (messageItems.get(i).getInfoType()) {
                                                            %>
                                                                    <span class="label label-success">成功</span>
                                                            <%
                                                                } else {
                                                            %>
                                                                    <span class="label label-pink">失败</span>
                                                            <%
                                                                }
                                                            %>
                                                        </td>
                                                        <td class="am-hide-sm-only"><%=messageItems.get(i).getClientIP()%></td>
                                                        <td class="am-hide-sm-only"><%=messageItems.get(i).getIssueTime().substring(0, 19)%></td>
                                                        <td>
                                                            <div class="am-btn-toolbar">
                                                                <div style="float: left;">
                                                                    <audio controls="controls" height="100" width="50">
                                                                        <source src="wavLib/1179276193420.wav" type="audio/wav" />
                                                                    </audio>
                                                                </div>
                                                                <div class="am-btn-group am-btn-group-xs" style="float: right;">
                                                                    <button class="am-btn am-btn-default am-btn-xs am-text-danger am-hide-sm-only">
                                                                        <span class="am-icon-trash-o"></span> 删除
                                                                    </button>
                                                                </div>
                                                            </div>
                                                        </td>
                                                    </tr>
                                            <%
                                                }
                                            %>
                                        </tbody>
                                    </table>
                                    <div class="am-cf">
                                        共 <%=messageItems.size()%> 条记录
                                        <div class="am-fr">
                                            <ul class="am-pagination">
                                                <li <%if(index == 1){%>class="am-disabled"<%}%>><a href="<%if(index!=1){%>table_complete.jsp?index=<%=index-1%><%}else{%>#<%}%>">«</a></li>
                                                <%
                                                    for (int i = index - 3; i <= index + 3; ++i) {
                                                        if (i > 0 && i <= pageLength) {
                                                            if (i == index) {
                                                %>
                                                                <li class="am-active"><a href="#"><%=i%></a></li>
                                                <%
                                                            } else {
                                                %>
                                                                <li><a href="table_complete.jsp?index=<%=i%>"><%=i%></a></li>
                                                <%
                                                            }
                                                        }
                                                    }
                                                %>
                                                <li <%if(index == pageLength){%>class="am-disabled"<%}%>><a href="<%if(index!=pageLength){%>table_complete.jsp?index=<%=index+1%><%}else{%>#<%}%>">»</a></li>
                                            </ul>
                                        </div>
                                    </div>
                                    <hr />
                                </form>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>

		<a href="admin-offcanvas" class="am-icon-btn am-icon-th-list am-show-sm-only admin-menu" data-am-offcanvas="{target: '#admin-offcanvas'}"></a>
        <script type="text/javascript">
            //checkbox 全选/取消全选
            var isCheckAll = false;
            function swapCheck() {
                if (isCheckAll) {
                    $("input[type='checkbox']").each(function() {
                        this.checked = false;
                    });
                    isCheckAll = false;
                } else {
                    $("input[type='checkbox']").each(function() {
                        this.checked = true;
                    });
                    isCheckAll = true;
                }
            }
        </script>
        <script type="text/javascript" src="assets/js/jquery-2.1.0.js" ></script>
		<script type="text/javascript" src="assets/js/amazeui.min.js"></script>
		<script type="text/javascript" src="assets/js/app.js" ></script>
		<script type="text/javascript" src="assets/js/blockUI.js" ></script>
	</body>
</html>
