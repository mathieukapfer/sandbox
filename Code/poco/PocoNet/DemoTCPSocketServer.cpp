#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/StreamSocket.h"

#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/TCPServer.h"
#include <iostream>

class MyTcpServerConnectionHandler : public Poco::Net::TCPServerConnection {

 public:
  MyTcpServerConnectionHandler(const Poco::Net::StreamSocket& socket):
    TCPServerConnection(socket)
    {};
  virtual ~MyTcpServerConnectionHandler() {};

  // redefine run
  virtual void run() {
      Poco::Net::SocketStream str(this->socket());

      // write
      str << "Hello guys" << std::flush;

      // read
      Poco::FIFOBuffer fifo(1024);
      this->socket().receiveBytes(fifo);
      std::cout << "I got the the message:" << fifo.begin() << std::endl;
  }
};

int main(int argc, char** argv) {

  Poco::Net::ServerSocket srs(8080); // does bind + listens
  Poco::Net::TCPServer srv
    (new Poco::Net::TCPServerConnectionFactoryImpl
     <MyTcpServerConnectionHandler>, srs); // create a thread to handle each client

  // Start TCP server
  srv.start();

  // wait forever
  while(1) { sleep(1); }

  // Stop the TCP Server
  srv.stop();

  return 0;
}
