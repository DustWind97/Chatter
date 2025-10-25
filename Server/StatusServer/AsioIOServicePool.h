#pragma once
#include <vector>
#include <boost/asio.hpp>					  // 1.81.0的老版用法
#include <boost/asio/executor_work_guard.hpp> // 1.87.0的新版用法
#include "Singleton.h"
#include "const.h"

class AsioIOServicePool:public Singleton<AsioIOServicePool>
{
	friend Singleton<AsioIOServicePool>;
public:
	using IOService = boost::asio::io_context;
	//using Work = boost::asio::io_context::work;//1.81.0的旧版用法
	//1.87.0的新版改成asio命名空间的executor_work_guard类
	using Work = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>; 
	using WorkPtr = std::unique_ptr<Work>;

	~AsioIOServicePool();											 // 析构函数
	AsioIOServicePool(const AsioIOServicePool&) = delete;			 // 拷贝构造（删除）
	AsioIOServicePool& operator=(const AsioIOServicePool&) = delete; // 拷贝赋值（删除）

	// 使用round-robin（轮询）的方式返回一个 io_service
	boost::asio::io_context& GetIOService();						 // 获取下一个IOService
	void Stop();													 // 停止所有IOService

private:
	// 构造函数，默认线程数为2或硬件并发数
	AsioIOServicePool(std::size_t size = 2/*std::thread::hardware_concurrency()*/);	
	std::vector<IOService>	 _ioServices;	 // IOService集合
	std::vector<WorkPtr>	 _works;		 // 工作集
	std::vector<std::thread> _threads;	     // 线程集合
	std::atomic<size_t>		 _nextIOService; // 轮询下一个IOService，原子操作
};

