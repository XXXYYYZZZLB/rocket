日志模块开发

日志要支持分级
日志支持打印到文件，支持文件日期命名，支持文件滚动（文件不会太长）
格式化风格C语言风格
多线程 保证线程安全

LogLevel 日志级别类:
Debug
Info
Error

LogEvent 打印什么东西:
行号
文件名
MsgNo rpc消息
线程进程号
日期以及时间精确到ms
自定义的字符串

日志的打印格式：
[%y-%m-%d %H:%M:%s.%ms]\t[pid:thread_id]\t[file_name:line][%msg]

Logger 日志器
1.提供打印日志的方法
2。提供日志的输出路径




EventLoop模块
Reactor是一种事件处理模型 反应堆模式
他的核心是一个Loop循环，在循环里不断调用epoll_wait监听套接字IO事件
一旦发送IO事件，epoll_wait返回，线程转而去处理这些IO事件
（一般是执行绑定在其的回调函数）
处理完事件以后，又重新陷入到epoll_wait中

