# ZeroNet

开发工具：Qt 5 MinGW<br>
服务端：Qt库<br>
客户端：Win32 API<br>
原作者：sumkee911<br>


ZeroNet 属于远程控制类软件，通过添加功能模块和完善代码逻辑，学习网络攻防和操作系统的相关知识，切勿作为黑客软件使用！！！<br>
使用该此程序进行网络攻击所造成的的后果由使用者承担！！！<br>

**安全性：**<br>
ZeroNet的客户端在原代码的基础上添加了打开确认机制，防止被人利用作为网络攻击工具<br>
若不小心点开了客户端且确认被控，手动清除方法：<br>
>1.删除此文件<br>
>2.删除D盘根目录下的同名文件<br>
>3.删除注册表的开机自启动项       HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run\ <br>
>4.任务管理器中将同名进程结束<br>
>5.重启电脑<br>


**功能更新概览：**<br>

2017.12.11  ZeroNet Sever 2.0<br>
![界面UI](https://github.com/joliph/ZeroNet/blob/master/ZeroNet2.jpg)<br>

1.修改界面图标，为扁平化风格优化，去除客户端列表的滚动显示条<br>
3.客户信息增为  id username ip port system sity<br>
2.增加DDOS模块（无界面，点击启动类型） ，用于测试服务器压力<br>
3.增加作者信息，优化程序执行逻辑<br>
4.增加大量功能的错误提示信息，方便后续调试修改<br>


2017.12.13  ZeroNet Client 1.2<br>
1.复制自身进入D盘<br>
2.将D盘根目录下的文件添加进入自启动列表<br>
3.添加安全确认机制<br>
4.修复互斥体bug<br>
5.DDOS功能接口完成<br>

2017.12.13  ZeroNet Sever 2.1<br>
1.添加DDOS窗口，统一功能界面，增加攻击端口选项<br>


2017.12.16  ZeroNet Client 2.0<br>
1.补完DDOS攻击代码<br>
2.源地址随机化处理<br>
3.修复bug<br>

2018.1.20   ZeroNet拟更新目标：<br>
1.测试外网上线，修复各种bug<br>
2.动态ip上线功能，解决ip地址变动不能上线的问题<br>
3.尝试移植控制端到安卓平台，实现手机监控电脑功能（这个实在是太棒啦～～我挺喜欢这个的）<br>


2017.12.16 ZeroNet Client 2.0<br>
1.外网上线支持内网映射啦，端口1为客户端的目标端口，端口2为映射后服务端的接收端口<br>
2.调用了126的接口,增加了ip地址的显示功能(ip地址所在城市或者显示内网)<br>
(若出现提示connect: Cannot connect (null)::aboutToQuit() to QNativeWifiEngine::closeHandle()，需要把D:\QT\Tools\mingw530_32\opt\bin目录下libeay32.dll和ssleay32.dll,两个dll文件拷贝到D:\coding\QT\5.7\mingw53_32\bin下)<br>

2018.5.4 ZeroNet Sever 2.2<br>
1.加入反调试，反沙箱，反虚拟机功能<br>
2.反调试：3环下进程遍历比对进程名，自身全路径敏感词检测，IsDebuggerPresent<br>
3.更新运行确认机制，确保无意被控的情况下进程立即结束<br>

目标更新Q.Q <br>
1.增加通讯加密功能<br>
2.增加自我保护功能，防止被3环进程结束<br>
3.Hook关键API，学习DLL注入技术（后面可能会把sever分离为exe和dll形式）<br>
4.进程检测能力提升，加入对进程隐藏技术的对抗能力<br>
5.编写本软件的卸载程序，防止误点击中招<br>
