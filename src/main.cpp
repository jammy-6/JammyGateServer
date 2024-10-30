#include "Global.h"
#include "CServer.h"
#include "HttpConnection.h"
#include "LogicSystem.h"

#include "ConfigMgr.h"
#include "RedisMgr.h"
#include "MysqlMgr.h"
void testRegest(){
    MysqlMgr::GetInstance()->RegUser("jjjjj","8888@qq.com","123123");
}
int main()
{
    ///testRegest();
    Log::Instance()->init(0,PROJECT_DIR);
    RedisMgr::GetInstance();

    
    try
    {

        // unsigned short port = 8080;
        std::string portString = gConfigMgr["GateServer"]["Port"];
        std::cout<<portString<<std::endl;
        unsigned short port = static_cast<unsigned short>(std::stoi(gConfigMgr["GateServer"]["Port"]));
        LOG_INFO("Server port is %d",static_cast<int>(port));
        net::io_context ioc{ 1 };
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code& error, int signal_number) {
            if (error) {
                return 0;
            }
            ioc.stop();
            return 0;
        });
        std::make_shared<CServer>(ioc, port)->Start();
        ioc.run();
        return 0;
    }
    catch (std::exception const& e)
    {
        LOG_ERROR(e.what());
        return EXIT_FAILURE;
    }
}

unsigned char ToHex(unsigned char x)
{
    return  x > 9 ? x + 55 : x + 48;
}

unsigned char FromHex(unsigned char x)
{
    unsigned char y;
    if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
    else if (x >= '0' && x <= '9') y = x - '0';
    else assert(0);
    return y;
}
std::string UrlEncode(const std::string& str)
{
    std::string strTemp;
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        //判断是否仅有数字和字母构成
        if (isalnum((unsigned char)str[i]) ||
            (str[i] == '-') ||
            (str[i] == '_') ||
            (str[i] == '.') ||
            (str[i] == '~'))
            strTemp += str[i];
        else if (str[i] == ' ') //为空字符
            strTemp += "+";
        else
        {
            //其他字符需要提前加%并且高四位和低四位分别转为16进制
            strTemp += '%';
            strTemp += ToHex((unsigned char)str[i] >> 4);
            strTemp += ToHex((unsigned char)str[i] & 0x0F);
        }
    }
    return strTemp;
}


