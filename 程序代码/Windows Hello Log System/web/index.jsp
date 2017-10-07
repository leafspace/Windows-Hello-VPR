<%@ page import="java.util.ArrayList" %>
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
		<link rel="icon" href="favicon.ico" type="image/x-icon"/>
		<title>Windows Hello后台Log系统</title>
		<link rel="stylesheet" href="assets/css/amazeui.css" />
		<link rel="stylesheet" href="assets/css/core.css" />
		<link rel="stylesheet" href="assets/css/menu.css" />
		<link rel="stylesheet" href="assets/css/index.css" />
		<link rel="stylesheet" href="assets/css/admin.css" />
		<link rel="stylesheet" href="assets/css/page/form.css" />
		<link rel="stylesheet" href="assets/css/component.css" />
		<link rel="stylesheet" href="assets/css/page/typography.css" />
	</head>

    <%
        User user = (User) request.getSession().getAttribute("user");
        if (user == null) {
            request.getRequestDispatcher("login.jsp").forward(request,response);
        }
        DatabaseProxyFactory databaseProxyFactory = new DatabaseProxyFactory();
        DatabaseProxyInterface databaseProxyInterface = databaseProxyFactory.getDatabaseProxy("MySQL");
        ArrayList<MessageItem> messageItems = databaseProxyInterface.getInfoList();
    %>

	<body>
		<header class="am-topbar am-topbar-fixed-top">
			<div class="am-topbar-left am-hide-sm-only">
				<a href="#" class="logo"><span style="font-family: '微软雅黑';">管理员</span><i class="zmdi zmdi-layers"></i></a>
			</div>

			<div class="contain">
				<ul class="am-nav am-navbar-nav am-navbar-left">
					<li><h4 class="page-title">基本信息列表</h4></li>
				</ul>
				<ul class="am-nav am-navbar-nav am-navbar-right">
					<li class="hidden-xs am-hide-sm-only">
						<form role="search" class="app-search">
							<input type="text" placeholder="查找..." class="form-control">
							<a href="#"><img src="assets/img/search.png"></a>
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
						<li><a href="#"><span class="am-icon-home"></span> 首页</a></li>
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
					<div class="am-g">
						<div class="am-u-md-8" style="width:100%;">
							<div class="card-box">
								<h4 class="header-title m-t-0 m-b-30">最新日志</h4>
								<div class="am-scrollable-horizontal am-text-ms" style="font-family: '微软雅黑';">
									<table class="am-table   am-text-nowrap">
										<thead>
											<tr>
												<th>ID</th>
												<th>客户端类型</th>
												<th>发布时间</th>
												<th>状态</th>
												<th>客户端IP</th>
											</tr>
										</thead>
										<tbody>
											<%
												for (int i = 0; i < Math.min(messageItems.size(), 15); ++i) {
											%>
												<tr onclick="window.location.href='form_validate.jsp?ID=<%=messageItems.get(i).getID()%>'">
													<td><%=messageItems.get(i).getID()%></td>
													<td><%=messageItems.get(i).getClientType()%></td>
													<td><%=messageItems.get(i).getIssueTime().substring(0, 19)%></td>
													<%if (messageItems.get(i).getInfoType()) {%>
													<td><span class="label label-success">成功</span></td>
													<%} else {%><td><span class="label label-pink">失败</span></td><%}%>
													<td><%=messageItems.get(i).getClientIP()%></td>
												</tr>
											<%
												}
											%>
										</tbody>
									</table>
								</div>
							</div>
						</div>
					</div>
				</div>
			</div>
		</div>

		<a href="admin-offcanvas" class="am-icon-btn am-icon-th-list am-show-sm-only admin-menu" data-am-offcanvas="{target: '#admin-offcanvas'}"></a>
		<script type="text/javascript">
            var websocket = null;
            if ('WebSocket' in window) {                                         //判断当前浏览器是否支持WebSocket
                websocket = new WebSocket("ws://localhost:8080/websocket");
            } else {
                alert('您的浏览器不支持WebSocket,某些功能无法使用!');
            }

            websocket.onerror = function () {                                    //连接发生错误的回调方法
                alert('WebSocket连接发生错误!');
            }

            websocket.onopen = function () {}                                    //连接成功建立的回调方法

            websocket.onmessage = function (event) {                             //接收到消息的回调方法
                //alert(event.data);
                handleMessage(event.data);
            }

            websocket.onclose = function () {                                    //连接关闭的回调方法
                //alert('WebSocket连接关闭!');
            }

            window.onbeforeunload = function () {
                //监听窗口关闭事件，当窗口关闭时，主动去关闭websocket连接
                //防止连接还没断开就关闭窗口，server端会抛异常
                closeWebSocket();
            }

            function handleMessage(innerHTML) {                                  //处理接受到的消息
                if (innerHTML == "refresh") {
                    location.reload();
                }
            }

            function closeWebSocket() {                                          //关闭WebSocket连接
                websocket.close();
            }

            function send(message) {                                             //发送消息
                websocket.send('refresh');
            }
		</script>
		<script type="text/javascript" src="assets/js/jquery-2.1.0.js" ></script>
		<script type="text/javascript" src="assets/js/amazeui.min.js"></script>
		<script type="text/javascript" src="assets/js/app.js" ></script>
		<script type="text/javascript" src="assets/js/blockUI.js" ></script>
		<script type="text/javascript" src="assets/js/charts/echarts.min.js" ></script>
		<script type="text/javascript" src="assets/js/charts/indexChart.js" ></script>
	</body>
</html>
