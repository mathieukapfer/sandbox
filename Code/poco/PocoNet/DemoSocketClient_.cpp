#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/SocketAddress.h"

#include <iostream>

int main(int argc, char** argv) {

  Poco::Net::SocketAddress adr("127.0.0.1", 8080);
  Poco::Net::StreamSocket ss;
  char buffer[1024];
  //Poco::FIFOBuffer fifo(1024);

  ss.connect(adr);

  // read
  ss.receiveBytes(buffer, sizeof(buffer)-1);
  //ss.receiveBytes(fifo);
  //str >> buffer;
  std::cout << buffer << std::endl;
  //std::cout << "I got the the message:" << fifo.begin() << std::endl;

  // write
  Poco::Net::SocketStream str(ss);
  str << "Hello" << std::flush;

  return 0;
}
