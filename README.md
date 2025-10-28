# Chatter

#### 1\.**项目描述**

这是一个全栈聊天项目。前端界面采用 Qt，后端服务采用 C++。

前端界面整体风格类似微信客户端。基于 Qt 实现包含注册，登录，聊天等多个界面，并通过 QSS 样式表优化界面效果，支持账号注册，好友添加，好友聊天等功能。气泡聊天框通过 QListWidget 模块实现好友列表，聊天列表及好友添加列表；通过 QGridLayout 和 QPainter 模块实现聊天气泡框；通过 QNetwork 模块实现 HTTP 和 TCP 服务。

后端服务器采取分布式设计，包含 GateServer 网关服务，VerifyServer 验证服务，StatusServer 状态服务以及多个 ChatServer 聊天服务。

后端服务器各服务间通过 grpc 通信。GateServer 网关对外采用 HTTP 服务，负责处理用户登录和注册功能。登录时 GateServer 从 StatusServer 查询聊天服务负载均衡，ChatServer 聊天服务则采用 Asio 实现 TCP 可靠长链接异步通信和转发, 采用多线程模式封装 iocontext 池提升并发性能。数据存储采用 MySQL 服务，并基于 mysqlconnector 库封装连接池，同时封装 Redis 连接池处理缓存数据，以及 grpc 连接池保证多服务并发访问。

经 JMeter 接口测试工具测试，单服务器支持 8000 连接，多服务器分布部署可支持 1W~2W 活跃用户。

#### 2\.**技术点**

**Asio 网络库**，**grpc**，**多线程**，**Redis**, **MySql**，**Qt 信号与槽**，**网络编程**，**设计模式**

#### 3\.**项目意义**

项目实现前端 MVC 解耦以及后端分布式管理，同时解决了高并发场景下单个服务连接数吃紧的情况，提升了自己对并发和异步的认知和处理能力等。

