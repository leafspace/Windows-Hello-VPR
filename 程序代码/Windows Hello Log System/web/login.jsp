<%@ page contentType="text/html;charset=UTF-8" language="java" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
	<head>
		<meta charset="UTF-8">
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<title></title>	
		<link rel="stylesheet" href="assets/css/core.css" />
		<link rel="stylesheet" href="assets/css/menu.css" />
		<link rel="stylesheet" href="assets/css/amazeui.css" />
		<link rel="stylesheet" href="assets/css/component.css" />
		<link rel="stylesheet" href="assets/css/page/form.css" />
	</head>
	<body>
		<div class="account-pages">
			<div class="wrapper-page">
				<div class="text-center">
	                <a href="login.jsp" class="logo"><span style="font-family: '微软雅黑';">后台管理</span></a>
	            </div>
	            
	            <div class="m-t-40 card-box">
	            	<div class="text-center">
	                    <h4 class="text-uppercase font-bold m-b-0">登 陆</h4>
	                </div>
	                <div class="panel-body">
	                	<form class="am-form" action="/login.do" method="post">
	                		<div class="am-g">
	                			<div class="am-form-group">
							      <input type="text" name="username" class="am-radius"  placeholder="请输入用户名...">
							    </div>
							
							    <div class="am-form-group form-horizontal m-t-20">
							      <input type="password" name="password" class="am-radius"  placeholder="请输入密码...">
							    </div>
							    
							    <div class="am-form-group ">
		                           	<label style="font-weight: normal;color: #999;">
								        <input type="checkbox" class="remeber" name="remeber"> 记住我
								    </label>
		                        </div>
		                        
		                        <div class="am-form-group ">
		                        	<button type="submit" class="am-btn am-btn-primary am-radius" style="width: 100%;height: 100%;">登 陆</button>
		                        </div>
	                		</div>
	                	</form>
	                </div>
	            </div>
			</div>
		</div>
	</body>
</html>
