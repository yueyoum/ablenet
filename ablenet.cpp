#include <vector>
#include <deque>
#include <string>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "ablenet.h"
#include "plugin.h"


ablenet::Client::Client(uint64_t id, tcp::socket socket, Plugin::Instance* pins)
    : Session(std::move(socket)),
      id_(id),
      pins_(pins)
{
}

ablenet::Client::~Client()
{
    delete pins_;
}

void ablenet::Client::on_data(std::string data)
{
    pins_->on_data(data);
}


std::mutex ablenet::Server::mutex_;
ablenet::Server* ablenet::Server::instance_;
ablenet::Server* ablenet::Server::instance(boost::asio::io_service& io)
{
    if(!instance_)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if(!instance_)
        {
            instance_ = new Server(io);
        }
    }

    printf("server inin\n");
    return instance_;
}

ablenet::Server* ablenet::Server::instance()
{
    return instance_;
}


ablenet::Server::Server(boost::asio::io_service& io)
    : accepter_(io, tcp::endpoint(tcp::v4(), 9090)),
      strand_(io)
{
    plugin_ = new Plugin::Plugin();
    is_started = false;
}

ablenet::Server::~Server()
{
    delete plugin_;
}

void ablenet::Server::start()
{
    if(is_started)
    {
        return;
    }

    auto x = [](boost::asio::yield_context yield, ablenet::Server* server) {
        for (; ;) {
            boost::system::error_code error;

            ++server->client_id_;
            tcp::socket socket(server->accepter_.get_io_service());
            printf("block for accept\n");
            server->accepter_.async_accept(socket, yield[error]);

            if(error)
            {
                std::cout << "accept error: " << error << std::endl;
                break;
            }

            printf("accept done...\n");

            auto client = std::make_shared<Client>(server->client_id_, std::move(socket), server->plugin_->new_instance(server->client_id_));

            std::weak_ptr<Client> wp(client);
            server->clients_[server->client_id_] = wp;
            client->start();
        }
    };

    boost::asio::spawn(
            strand_,
            std::bind(x, std::placeholders::_1, this)
    );

    is_started = true;
    printf("Server Start Run...\n");
}

void ablenet::Server::send(uint64_t id, std::string data)
{
    clients_[id].lock()->send(data);
}

void ablenet::Server::broadcast(std::string data)
{
    for(auto const &item: clients_)
    {
        item.second.lock()->send(data);
    }
}


int main() {
    std::cout << "Start..." << std::endl;

    boost::asio::io_service io;
    auto server = ablenet::Server::instance(io);

    boost::asio::signal_set signal_set(io, SIGINT);
    signal_set.async_wait([&io](const boost::system::error_code &, int) {
            std::cout << "<SIGINT>\n";
            io.stop();
            });

    server->start();
    io.run();

    printf("DONE\n");
    return 0;
}
