#include "Message.hpp"
#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

int main() {
  try {
    boost::asio::io_context io_context;
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve("127.0.0.1", "10013");
    tcp::socket socket(io_context);
    boost::asio::connect(socket, endpoints);
    {
      Message msg = Person{"John Doe", 1234567890, 42, "john@example.com"};
      std::ostringstream stream;
      boost::archive::text_oarchive oa(stream);
      oa << msg;
      boost::asio::write(socket, boost::asio::buffer(stream.str()));
      std::array<char, 128> buffer;
      size_t bytes_transferred = socket.read_some(boost::asio::buffer(buffer));
      std::cout << "Received response from server: "
                << std::string(buffer.data(), bytes_transferred) << std::endl;
    }
    {
      Message msg = Company{"Nasa", 9876543210};
      std::ostringstream stream;
      boost::archive::text_oarchive oa(stream);
      oa << msg;
      boost::asio::write(socket, boost::asio::buffer(stream.str()));
      std::array<char, 128> buffer;
      size_t bytes_transferred = socket.read_some(boost::asio::buffer(buffer));
      std::cout << "Received response from server: "
                << std::string(buffer.data(), bytes_transferred) << std::endl;
    }
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
  return 0;
}
