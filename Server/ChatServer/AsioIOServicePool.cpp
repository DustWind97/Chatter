#include "AsioIOServicePool.h"
#include <iostream>
using namespace std;
AsioIOServicePool::AsioIOServicePool(std::size_t size)
	:_ioServices(size)
	,_works(size)
	,_nextIOService(0)
{
	// 创建工作集
	for (std::size_t i = 0; i < size; ++i) {
		//1.81.0的旧版用法
		//_works[i] = std::unique_ptr<Work>(new Work(_ioServices[i]));
		//1.87.0的新版用法
		//_works[i] = std::make_unique<Work>(_ioServices[i].get_executor());//两种写法等价的
		_works[i] = std::unique_ptr<Work>(new Work(_ioServices[i].get_executor()));
	}

	//遍历多个ioservice，创建多个线程，每个线程内部启动ioservice
	for (std::size_t i = 0; i < _ioServices.size(); ++i) {
		_threads.emplace_back([this, i]() {
			_ioServices[i].run();
			});
	}
}

AsioIOServicePool::~AsioIOServicePool() {
	Stop();
	std::cout << "AsioIOServicePool destruct" << std::endl;
}

boost::asio::io_context& AsioIOServicePool::GetIOService() {
	auto& service = _ioServices[_nextIOService++];
	if (_nextIOService == _ioServices.size()) {
		_nextIOService = 0;
	}
	return service;
}

//优雅关闭机制
void AsioIOServicePool::Stop(){
	//因为仅仅执行work.reset并不能让iocontext从run的状态中退出
	//当iocontext已经绑定了读或写的监听事件后，还需要手动stop该服务。

	////1.81.0的老版用法
	//for (auto& work : _works) {
	//	//把服务先停止
	//	work->get_io_context().stop();
	//	//再将io池重置
	//	work.reset();
	//}
	////1.87.0的新版用法
	////把服务先停止
	////遍历多个ioservice的线程，每个线程内部停止ioservice
	//for (std::size_t i = 0; i < _ioServices.size(); ++i) {
	//	_threads.emplace_back([this, i]() {
	//		_ioServices[i].stop();
	//	});
	//}

	// 1.停止所有IO服务
	for (auto& service : _ioServices) 
	{
		service.stop(); //同步停止操作
	}

	// 2.释放work对象
	for (auto& work : _works) 
	{
		work.reset();
	}

	// 3.等待工作线程退出
	for (auto& thread : _threads) 
	{
		/*thread.join();*/
		if (thread.joinable()) thread.join();
	}
	_threads.clear(); // 清空线程容器
}
