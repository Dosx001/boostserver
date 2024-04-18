#include "Message.hpp"
#include <boost/archive/text_iarchive.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <sstream>
#include <string>

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
  explicit Session(tcp::socket socket) : socket_(std::move(socket)), data_{} {}
  void start() { handle_msg(); }

private:
  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  void handle_msg() {
    auto self(shared_from_this());
    socket_.async_read_some(
        boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length) {
          if (!ec) {
            std::istringstream stream(std::string(data_, length));
            boost::archive::text_iarchive ia(stream);
            Message msg;
            ia >> msg;
            switch (msg.which()) {
            case 0: {
              const Person p = boost::get<Person>(msg);
              std::cout << "Person: " << p.name << "\n"
                        << "ID: " << p.id << "\n"
                        << "Age: " << p.age << "\n"
                        << "Email: " << p.email << std::endl;
              break;
            }
            case 1: {
              const Company c = boost::get<Company>(msg);
              std::cout << "Company: " << c.name << "\n"
                        << "ID: " << c.id << std::endl;
              break;
            }
            default:
              std::cout << "Unknown type" << std::endl;
              break;
            }
            send_msg(length);
          }
        });
  }
  void send_msg(std::size_t length) {
    auto self(shared_from_this());
    boost::asio::async_write(
        socket_, boost::asio::buffer(data_, length),
        [this, self](boost::system::error_code ec, std::size_t) {
          if (!ec) {
            handle_msg();
          }
        });
  }
};

class Server {
public:
  Server(boost::asio::io_context &io_context, short port)
      : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
    listen();
  }

private:
  tcp::acceptor acceptor_;
  void listen() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
          if (!ec) {
            std::make_shared<Session>(std::move(socket))->start();
          }
          listen();
        });
  }
};

int main() {
  try {
    boost::asio::io_context io_context;
    Server s(io_context, 10013);
    io_context.run();
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  return 0;
}
