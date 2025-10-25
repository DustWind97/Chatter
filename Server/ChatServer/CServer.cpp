#include "CServer.h"
#include "const.h"
#include "AsioIOServicePool.h"
#include "UserMgr.h"
#include "ConfigMgr.h"
#include "RedisMgr.h"

CServer::CServer(boost::asio::io_context& io_context, short port)  
	:_io_context(io_context)
	,_port(port)
	,_acceptor(io_context, tcp::endpoint(tcp::v4(), port)) // 初始化acceptor监听  
    ,_timer(io_context) // 初始化_timer  
{  
	std::cout << "Server start success, listen on port : " << _port << std::endl;  
	StartAccept();  
}

//析构函数
CServer::~CServer()
{
}

//Session无效时从map中清除这一个Session
void CServer::ClearSession(std::string session_id)
{
	if (_sessions.find(session_id) != _sessions.end()) //查到做移除操作
    {
        //从UserMgr中移除用户会话绑定
		UserMgr::GetInstance()->RemoveUserSession(_sessions[session_id]->GetUserId());
	}
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _sessions.erase(session_id);
    }
}

std::shared_ptr<CSession> CServer::GetSession(std::string uuid)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _sessions.find(uuid);
    if (it != _sessions.end()) {
        return it->second;
    }
    return nullptr;
}

bool CServer::CheckValid(std::string uuid)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _sessions.find(uuid);
    if (it != _sessions.end()) {
        return true;
    }
    return false;
}

void CServer::on_timer(const boost::system::error_code& ec)
{
	if (ec) {
		std::cout << "timer error: " << ec.message() << std::endl;
		return;
	}
	std::vector<std::shared_ptr<CSession>> _expired_sessions;
	int session_count = 0;
	//此处加锁遍历session
	std::map<std::string, std::shared_ptr<CSession>> sessions_copy;
	{
		std::lock_guard<std::mutex> lock(_mutex);
		sessions_copy = _sessions;
	}

	time_t now = std::time(nullptr);
	for (auto iter = sessions_copy.begin(); iter != sessions_copy.end(); iter++) {
		auto b_expired = iter->second->IsHeartbeatExpired(now);
		if (b_expired) {
			//关闭socket, 其实这里也会触发async_read的错误处理
			iter->second->Close();
			//收集过期信息
			_expired_sessions.push_back(iter->second);
			continue;
		}
		session_count++;
	}

	//设置session数量
	auto& cfg = ConfigMgr::Inst();
	auto self_name = cfg["SelfServer"]["Name"];
	auto count_str = std::to_string(session_count);
	RedisMgr::GetInstance()->HSet(LOGIN_COUNT, self_name, count_str);

	//处理过期session, 单独提出，防止死锁
	for (auto& session : _expired_sessions) {
		session->DealExceptionSession();
	}

	//再次设置，下一个60s检测
	_timer.expires_after(std::chrono::seconds(60));
	_timer.async_wait([this](boost::system::error_code ec) {
		on_timer(ec);
		});
}

void CServer::StartTimer()
{
	//启动定时器
	auto self(shared_from_this());
	_timer.async_wait([self](boost::system::error_code ec) {
		self->on_timer(ec);
		});
}

void CServer::StopTimer()
{
	_timer.cancel();
}

//处理接收连接
void CServer::HandleAccept(std::shared_ptr<CSession> new_session, const boost::system::error_code& error)
{
    if (!error)
    {
        new_session->Start();
        std::lock_guard<std::mutex> lock(_mutex);
        _sessions.insert(std::make_pair(std::to_string(new_session->GetUserId()), new_session));
    }
	else
	{
		std::cout << "session accept failed, error is: " << error.what() << std::endl;
	}

    StartAccept();
}

//开始接收连接
void CServer::StartAccept() {
    auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
    std::shared_ptr<CSession> new_session = std::make_shared<CSession>(io_context, this);
    _acceptor.async_accept(new_session->GetSocket(), 
                           std::bind(&CServer::HandleAccept, this, new_session, std::placeholders::_1));//placeholders为占位符
}