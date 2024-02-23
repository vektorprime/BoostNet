#include <iostream>
#include <string>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
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
///
//////////////////////////
class tcp_connection
	: public boost::enable_shared_from_this<tcp_connection>
{
public:
	typedef boost::shared_ptr<tcp_connection> pointer;

	static pointer create(boost::asio::io_context &io_context)
	{
		return pointer(new tcp_connection(io_context));
	}

	tcp::socket &socket()
	{
		return socket_;
	}

	void start()
	{
		message_ = "test";

		boost::asio::async_write(
			socket_, 
			boost::asio::buffer(message_),
			boost::bind(&tcp_connection::handle_write, 
			shared_from_this(), 
			boost::asio::placeholders::error, 
			boost::asio::placeholders::bytes_transferred));
	}

private:
	tcp_connection(boost::asio::io_context &io_context)
		: socket_(io_context)
	{
	}

	void handle_write(const boost::system::error_code & /*error*/,
		size_t /*bytes_transferred*/)
	{
	}

	tcp::socket socket_;
	std::string message_;
};

class tcp_server
{
public:
	tcp_server(boost::asio::io_context &io_context)
		: io_context_(io_context),
		acceptor_(io_context, tcp::endpoint(tcp::v4(), 1337))
	{
		start_accept();
	}

private:
	void start_accept()
	{
		tcp_connection::pointer new_connection = tcp_connection::create(io_context_);

		acceptor_.async_accept(
			new_connection->socket(),
			boost::bind(&tcp_server::handle_accept, this, new_connection, boost::asio::placeholders::error)
		);
	}

	void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code &error)
	{
		if (!error)
		{
			new_connection->start();
		}

		start_accept();
	}

	boost::asio::io_context &io_context_;
	tcp::acceptor acceptor_;
};
/////////////

int main()
{
	//All programs that use asio need to have at least one I/O execution context, such as an io_context object. 
	boost::asio::io_context io_context{};
	tcp_server server(io_context);
	io_context.run();



	//std::cout << "Listenning for incoming connections" << std::endl;
	//bool listen_for_conn = true;
	//while (listen_for_conn)
	//{
	//	std::thread worker{};
	//	//Create a socket that will represent the connection to the client, and then wait for a connection. 
	//	tcp::socket socket(io_context);
	//	//Wait for a conneciton request (block)
	//	acceptor.accept(socket);
	//	std::cout << "Connection accepted" << std::endl;
	//	worker = std::thread(process_connection, std::move(socket));
	//	worker.detach();
	//}



	return 0;
}
