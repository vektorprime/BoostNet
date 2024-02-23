#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <thread>

using boost::asio::ip::tcp;

std::mutex mux{};

void process_connection(tcp::socket &&socket)
{
	//If we made it this far a client is accessing our socket
	//send something back
	//boost::system::error_code ignored_error;
	while (true)
	{
		std::array<char, 128> buf{};
		boost::system::error_code error{};
		std::scoped_lock lock(mux);
		std::string message{};
		//zero out len in case old data left in there masks error
		size_t len = 0;
		len = socket.read_some(boost::asio::buffer(buf), error);
		//if any data in len, assume conn is good
		if (len)
		{
			std::cout << "Incomming message" << std::endl;
			std::cout.write(buf.data(), len);
			std::cout << std::endl;
			//boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
		}
		else
		{
			//any kind of error shouldn't throw here, just close the thread by breaking the while loop
			if (error)
			{
				std::cout << "Connection closed" << std::endl;
				break;
			}
		}
		
	
	}
}

int main()
{
	//All programs that use asio need to have at least one I/O execution context, such as an io_context object. 
	boost::asio::io_context io_context{};
	io_context.run();

	// A ip::tcp::acceptor object needs to be created to listen for new connections. It is initialised to listen on TCP port 1337, for IP version 4. 
	tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 1337));

	

	std::cout << "Listenning for incoming connections" << std::endl;
	bool listen_for_conn = true;
	while (listen_for_conn)
	{
		std::thread worker{};
		//Create a socket that will represent the connection to the client, and then wait for a connection. 
		tcp::socket socket(io_context);
		//Wait for a conneciton request (block)
		acceptor.accept(socket);
		std::cout << "Connection accepted" << std::endl;
		worker = std::thread(process_connection, std::move(socket));
		worker.detach();
	}



	return 0;
}
