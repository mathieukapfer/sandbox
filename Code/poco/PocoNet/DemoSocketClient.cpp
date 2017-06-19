#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/SocketAddress.h"

#include <iostream>

int main(int argc, char** argv) {

  Poco::Net::SocketAddress adr("127.0.0.1", 8080);
  Poco::Net::StreamSocket ss;
  Poco::FIFOBuffer fifo(1024);

  // connect
  ss.connect(adr);

  // read
  ss.receiveBytes(fifo);
  std::cout << "I got the the message:" << fifo.begin() << std::endl;

  // simulate processing 
  sleep(1);

  // write
  Poco::Net::SocketStream str(ss);
  str << "Hello" << std::flush;

  return 0;
}
