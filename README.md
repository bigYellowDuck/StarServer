StarServer
===
StarServer是一个稍显简陋的异步非阻塞网络库，使用`C++11`开发。<br>StarServer设计目标是：使用简单。

示例：一个echo服务器
----
```cpp

Server sever(12345);       // 监听12345端口
server.setThreadNum(4);    // 设置4个工作线程，不设置则在IO线程里进行事件处理
server.setConnectionCallback(    // 设置新连接回调
    [](const TcpConnectionPtr& conn) {
      if (conn->connected()) {
        printf("%s connectionCallback:new connection %d.\n", util::moment().data(), conn->connId());
      } else {
        printf("failed\n");
      }
    }); 

server.setMessageCallback(       // 设置读入数据回调
    [](const TcpConnectionPtr& conn, ReadBuffer* buffer){
      printf("%s connection %d messageCallback:received %ld bytes from connection\n", util::moment().data(), conn->connId(), buffer->len());
      const string str = buffer->retrieveAsString();
      conn->send(str);
    });
    
server.start();    // 启动服务器
```
