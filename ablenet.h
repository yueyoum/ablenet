#ifndef ABLENET_H_INCLUDE
#define ABLENET_H_INCLUDE

#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

#include "session.h"
#include "plugin.h"

namespace ablenet
{

using boost::asio::ip::tcp;


class Client : public Session {
public:
    Client(uint64_t id, tcp::socket socket, Plugin::Instance* pins);
    ~Client();

    void on_data(std::string);

private:
    uint64_t id_;
    Plugin::Instance* pins_;
};


class Server {
public:
    static Server* instance();
    static Server* instance(boost::asio::io_service&);
    ~Server();
    void start();
    void send(uint64_t, std::string);
    void broadcast(std::string);

    void remove_client(uint64_t);

private:
    static std::mutex mutex_;
    static Server* instance_;

    tcp::acceptor accepter_;
    boost::asio::io_service::strand strand_;
    uint64_t client_id_;
    Plugin::Plugin *plugin_;

    bool is_started;

    std::map<uint64_t, std::weak_ptr<Client>> clients_;

    Server(boost::asio::io_service&);
    Server(const Server&) = delete;
    Server& operator = (const Server&) = delete;
};

}

#endif //ABLENET_H_INCLUDE
