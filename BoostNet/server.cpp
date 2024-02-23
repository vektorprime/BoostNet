#include <iostream>
#include <string>
#include <boost/asio.hpp>


using boost::asio::ip::tcp;


int main()
{
	//All programs that use asio need to have at least one I/O execution context, such as an io_context object. 
	boost::asio::io_context io_context{};

	// A ip::tcp::acceptor object needs to be created to listen for new connections. It is initialised to listen on TCP port 1337, for IP version 4. 
	tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 1337));

	std::cout << "Beginning to listen for connections" << std::endl;
	bool listen_for_conn = true;
	while (listen_for_conn)
	{
		//Create a socket that will represent the connection to the client, and then wait for a connection. 
		tcp::socket socket(io_context);
		//Wait for a conneciton request (block)
		acceptor.accept(socket);
		//If we made it this far a client is accessing our socket
		//send something back
		std::string message = "Can you hear us buddy?";
		boost::system::error_code ignored_error;
		while (true)
		{
			boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
		}

	}

	return 0;
}
