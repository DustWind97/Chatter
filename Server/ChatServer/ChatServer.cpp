#include "LogicSystem.h"
#include <csignal>//信号类
#include <thread>//线程类
#include <mutex>//互斥量类
#include "AsioIOServicePool.h"
#include "CServer.h"
#include "ConfigMgr.h"
#include "RedisMgr.h"
#include "ChatServiceImpl.h"

bool bstop = false;
std::condition_variable cond_quit;
std::mutex mutex_quit;

int main()
{
    //Redis实例化
    RedisMgr::GetInstance();

    //获取本服务器信息 
    auto& cfg = ConfigMgr::Inst();
	auto server_name = cfg["SelfServer"]["Name"];
    try {
        auto pool = AsioIOServicePool::GetInstance();
        //将登录数设置为0
        RedisMgr::GetInstance()->HSet(LOGIN_COUNT, server_name, "0");//服务器启动清零
        Defer derfer([server_name]() {
            RedisMgr::GetInstance()->HDel(LOGIN_COUNT, server_name);//关闭时删除登录数量
            RedisMgr::GetInstance()->Close();//关闭Redis连接池
            });

        //创建一个Grpc Server
        std::string server_address(cfg["SelfServer"]["Host"]+":"+cfg["SelfServer"]["RPCPort"]);//127.0.0.1:50055
		ChatServiceImpl service;
		grpc::ServerBuilder builder;
        // 监听端口并注册Grpc服务
        builder.AddListeningPort(server_address, ::grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        // 构建并启动Grpc服务器
        std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
        std::cout << "RPC Server listening on " << server_address << std::endl;
        //单独启动一个线程处理grpc服务
        std::thread  grpc_server_thread([&server]() {
            server->Wait();
            });


        std::cout << "ChatServer1 已经启动" << std::endl;

        //通过ASIO线程池来处理TCP IO事件
        boost::asio::io_context  io_context;
        //SIGINT信号通过Ctrl+C触发，SIGTERM信号可理解为关闭窗口触发
		boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&io_context, pool, &server](auto, auto) {
			io_context.stop();//停止io_context的事件循环
			pool->Stop();//停止线程池
			server->Shutdown();//关闭grpc服务
            });
       
        auto port_str = cfg["SelfServer"]["Port"];
        CServer s(io_context, atoi(port_str.c_str()));
        io_context.run();//如果没有绑定监听连接事件，run会自己释放掉

		grpc_server_thread.join();//等待grpc服务线程结束
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

}