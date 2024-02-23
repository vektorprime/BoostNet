#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using boost::asio::ip::tcp;


int main()
{
	//All programs that use asio need to have at least one I/O execution context, such as an io_context object. 
	boost::asio::io_context io_context{};


	std::string server_name;
	std::cout << "Enter the server name or IP" << std::endl;
	std::cin >> server_name;

	//We need to turn the server name that was specified as a parameter to the application, into a TCP endpoint. To do this we use an ip::tcp::resolver object. 
	tcp::resolver resolver(io_context);
	std::string port = "1337";
//	tcp::resolver::query query(server_name, port);
	//A resolver takes a host name and service name and turns them into a list of endpoints.
	//The list of endpoints is returned using an object of type ip::tcp::resolver::results_type.
	// This object is a range, with begin() and end() member functions that may be used for iterating over the results.
	tcp::resolver::results_type endpoint = resolver.resolve(server_name, port);
	
	bool listen_for_conn = true;

	//Now we create and connect the socket. The list of endpoints obtained above may contain both IPv4 and IPv6 endpoints, so we need to try each of them until we find one that works. 
	//This keeps the client program independent of a specific IP version. 
	//The boost::asio::connect() function does this for us automatically. 
	tcp::socket socket(io_context);
	boost::asio::connect(socket, endpoint);
	std::cout << "Connected" << std::endl;
	while (true)
	{
		//std::array<char, 128> buf{};
		//boost::system::error_code error;
		
		std::string message;
		std::cin >> message;
		boost::system::error_code ignored_error;

		boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
		if (!message.empty())
		{
			std::cout << "Sent message" << std::endl;
		}

		//size_t len = socket.read_some(boost::asio::buffer(buf), error);

		//if (error == boost::asio::error::eof)
		//{
		//	//break; // Connection closed cleanly by peer.
		//}
		//else if (error)
		//	throw boost::system::system_error(error); // Some other error.

		//std::cout.write(buf.data(), len);
	}



	return 0;
}