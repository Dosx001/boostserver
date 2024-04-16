#include "Person.hpp"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
  Session(tcp::socket socket) : socket_(std::move(socket)) {}

  void start() { doRead(); }

private:
  void doRead() {
    auto self(shared_from_this());
    socket_.async_read_some(
        boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length) {
          if (!ec) {
            std::istringstream archive_stream(std::string(data_, length));
            boost::archive::text_iarchive archive(archive_stream);
            Person receivedPerson;
            archive >> receivedPerson;
            std::cout << "Received: name=" << receivedPerson.name
                      << ", age=" << receivedPerson.age
                      << ", id=" << receivedPerson.id
                      << ", email=" << receivedPerson.email
                      << ", hash=" << receivedPerson.hash
                      << std::endl;
            doWrite(receivedPerson);
          }
        });
  }

  void doWrite(const Person &person) {
    auto self(shared_from_this());
    std::ostringstream archive_stream;
    boost::archive::text_oarchive archive(archive_stream);
    archive << person;
    std::string serialized_data = archive_stream.str();
    boost::asio::async_write(
        socket_, boost::asio::buffer(serialized_data),
        [this, self](boost::system::error_code ec, std::size_t) {
          if (!ec) {
            doRead();
          }
        });
  }

  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
};

class Server {
public:
  Server(boost::asio::io_context &io_context, short port)
      : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
        socket_(io_context) {
    doAccept();
  }

private:
  void doAccept() {
    acceptor_.async_accept(socket_, [this](boost::system::error_code ec) {
      if (!ec) {
        std::make_shared<Session>(std::move(socket_))->start();
      }
      doAccept();
    });
  }

  tcp::acceptor acceptor_;
  tcp::socket socket_;
};

int main() {
  try {
    boost::asio::io_context io_context;
    Server server(io_context, 8888);
    io_context.run();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
