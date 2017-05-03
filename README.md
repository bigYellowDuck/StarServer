![](doc/logo.png)
# StarServer
StarServer是一个稍显简陋的异步非阻塞网络库(poll(2)/epoll(7))，使用`C++11`开发。<br>StarServer设计目标是：使用简单。

### 安装
```cpp
sudo make
sudo make install
```


### 示例：一个echo服务器
```cpp

Server sever(12345);       // 监听12345端口，默认使用poll(2)驱动
                           // 若想使用epoll(7)驱动，要这样定义Server server(12345, "Epoller")
server.setThreadNum(4);    // 设置4个工作线程，不设置则在IO线程里进行事件处理
server.setMessageCallback(       // 设置读入数据回调
      [](const TcpConnectionPtr& conn, ReadBuffer* buffer) {
      const string str = buffer->retrieveAsString();
      conn->send(str);
    });
   
server.start();    // 启动服务器
```
### 简陋性能测试（没有太多参考价值）
example/echo的性能测试:<br>
<br>
联机测试<br>
测试工作: ab<br>
服务器主机 CPU: 速龙x2 270 双核<br>
客户端主机 Windows下的虚拟机 单核<br>
两台机器间的带宽：40MBits/sec(iperf测试结果)<br>
测试命令: ab -k -n200000 -c4 192.168.1.195:12345/<br>
每条信息长度:107byte<br>
吞吐量: 1050 requests/sec<br>
服务器主机echo的CPU使用率:20%(网速跟不上)<br>
<br>
单机测试<br>
吞吐量: 平均42000 requests/sec<br>
CPU使用率: echo的4个工作线程各占40%, ab占40%<br>


### 将会完善的事情
- ~~使用poll(2)机制实现Poller calss~~
- 定时器
- ~~安全退出~~
- 集成HTTP服务器
- ~~简陋性能测试~~
