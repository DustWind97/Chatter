#include <boost/asio.hpp>
#include <memory.h>
#include <map>
#include <mutex>
#include "CSession.h"

using boost::asio::ip::tcp;

class CServer : public std::enable_shared_from_this<CServer>
{
public:
    CServer(boost::asio::io_context& io_context, short port);
    ~CServer();
    void ClearSession(std::string uuid);//Session无效时从map中清除这一个Session
    //根据uid获取session
    std::shared_ptr<CSession> GetSession(std::string uuid);//获取会话
	bool CheckValid(std::string uuid); //检查会话有效性
	void on_timer(const boost::system::error_code& ec); //定时器回调函数
    void StartTimer(); //开始计时
    void StopTimer(); //停止计时

private:
    void HandleAccept(std::shared_ptr<CSession> new_session, const boost::system::error_code& error);
    void StartAccept();
    boost::asio::io_context& _io_context;
    short _port;
    tcp::acceptor _acceptor;//接收连接
    std::map<std::string, std::shared_ptr<CSession>> _sessions;//会话
    std::mutex _mutex;//线程锁
    boost::asio::steady_timer _timer;//定时器
};