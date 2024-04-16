#include "Message.hpp"
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

    Message personMsg = {
        PERSON,
        Person{
            "John Doe",
            42,
            1234567890,
            "john@example.com",
        },
    };
    std::ostringstream archive_stream;
    boost::archive::text_oarchive archive(archive_stream);
    archive << personMsg;
    std::string serialized_data = archive_stream.str();
    std::cout << "serialized_data: " << serialized_data << std::endl;
    boost::asio::write(socket, boost::asio::buffer(serialized_data));

    Message companyMsg = {
        COMPANY,
        Company{
            "Company Name",
            1234567890,
        },
    };
    std::ostringstream archive_stream2;
    boost::archive::text_oarchive archive2(archive_stream2);
    archive2 << companyMsg;
    std::string serialized_data2 = archive_stream2.str();
    std::cout << "serialized_data: " << serialized_data2 << std::endl;
    boost::asio::write(socket, boost::asio::buffer(serialized_data2));

    io_context.run();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
