﻿安装说明：
    数据库配置（默认情况）：
        数据库名：windowshello-vpr-logsystem
        用户名：root
        用户密码：123456
        数据库地址：127.0.0.1
    拷贝Windows Hello - VPR文件夹至你想要的位置
    拷贝Windows Hello Log System下的Windows Hello LogSystem.war夹至服务器路径下

    修改Windows Hello - VPR下info.conf的server和path选项为web服务器的地址跟web系统保存文件的位置
    修改注册表导入/setup.reg的Debugger路径为Windows Hello Application.exe的路径
    修改Windows Hello Log System/WEB-INF/info.json中的数据库服务器配置

注：
    若在一次web服务器启动后使用过一次websocket，请关闭浏览器后再打开网页才能够再次使用。同时在一个浏览器中不间断使用两次websocket。
此情况目前只限Google chrome