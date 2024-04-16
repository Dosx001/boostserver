#include "Person.hpp"
#include <boost/archive/text_oarchive.hpp>
#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

int main() {
  try {
    boost::asio::io_context io_context;
    tcp::socket socket(io_context);
    socket.connect(
        tcp::endpoint(boost::asio::ip::address::from_string("0.0.0.0"), 8888));
    Person personToSend = {"John Doe", 42, 1234567890, "john@example.com",
                           1234567890.1234567890};
    std::ostringstream archive_stream;
    boost::archive::text_oarchive archive(archive_stream);
    archive << personToSend;
    std::string serialized_data = archive_stream.str();
    boost::asio::write(socket, boost::asio::buffer(serialized_data));
    io_context.run();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
