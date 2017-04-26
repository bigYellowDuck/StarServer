![](doc/logo.png)
# StarServer
StarServer是一个稍显简陋的异步非阻塞网络库(使用epoll(7))，使用`C++11`开发。<br>StarServer设计目标是：使用简单。

### 安装
```cpp
sudo make
sudo make install
```


### 示例：一个echo服务器
```cpp

Server sever(12345);       // 监听12345端口
server.setThreadNum(4);    // 设置4个工作线程，不设置则在IO线程里进行事件处理
server.setMessageCallback(       // 设置读入数据回调
      [](const TcpConnectionPtr& conn, ReadBuffer* buffer) {
      const string str = buffer->retrieveAsString();
      conn->send(str);
    });
    
server.start();    // 启动服务器
```

### 将会完善的事情

- 使用poll(2)机制实现Poller calss
- 定时器
- ~~安全退出~~
- 集成HTTP服务器
- 性能测试
