#ifndef SESSION_H_INCLUDE
#define SESSION_H_INCLUDE

#include <iostream>
#include <memory>
#include <vector>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

namespace ablenet
{

using boost::asio::ip::tcp;


class Session : public std::enable_shared_from_this<Session>
{
public:
    explicit Session(tcp::socket);
    virtual ~Session();

    void send(std::string);
    virtual void start();

private:
    void recv(boost::asio::yield_context);
    void do_send(std::string data);
    virtual void on_data(std::string);

    tcp::socket socket_;
    boost::asio::strand strand_;
};


}

#endif //SESSION_H_INCLUDE
