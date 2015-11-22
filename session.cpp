#include "session.h"

ablenet::Session::Session(tcp::socket socket)
    : socket_(std::move(socket)),
      strand_(socket_.get_io_service())
{
}

ablenet::Session::~Session()
{
    socket_.close();
}

void ablenet::Session::send(std::string data)
{
    strand_.post(std::bind(&Session::do_send, shared_from_this(), data));
}

void ablenet::Session::on_data(std::string data)
{
}

void ablenet::Session::start()
{
    boost::asio::spawn(
            strand_,
            std::bind(&Session::recv, shared_from_this(), std::placeholders::_1)
            );
}

void ablenet::Session::recv(boost::asio::yield_context yield)
{
    std::array<char, 4096> buf;
    boost::system::error_code error;
    for(;;) {
        buf.fill('\0');
        error.clear();

        socket_.async_read_some(boost::asio::buffer(buf), yield[error]);

        if (error) {
            std::cout << "read error:" << error << std::endl;
            break;
        }

        on_data(std::string(buf.data()));
    }
}

void ablenet::Session::do_send(std::string data)
{
    auto callback = [](
            const boost::system::error_code& error,
            size_t bytes_transferred,
            std::shared_ptr<Session> sptr
            )
    {
        if(error)
        {
            std::cout << "write error: " << error << std::endl;
        }
    };

    boost::asio::async_write(
            socket_,
            boost::asio::buffer(data),
            std::bind(
                callback,
                std::placeholders::_1,
                std::placeholders::_2,
                shared_from_this()
                )
            );
}
