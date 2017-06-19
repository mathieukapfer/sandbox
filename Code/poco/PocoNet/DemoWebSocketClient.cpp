
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/WebSocket.h"

#include <iostream>

void EchoLoop(int nbRepetition, Poco::Net::WebSocket ws) {
  int repetition = 0;
  int flags = 0;
  int n = 0;
  int count = 0;
  std::cout << "Webssocket client msg processing start" << std::endl;
  do { //loop for connection handling
    char buffer[4096] = { 0 };
    int msgSize = 0;
    do { //loop for message
      std::cout << "Webssocket client msg processing waiting" << std::endl;
      n = ws.receiveFrame(buffer, sizeof(buffer), flags);
      std::cout << "Webssocket client msg processing recv:" << buffer << std::endl;
      msgSize +=n;
    } while ( n > 0 && ( (flags & 0xf0 /* masking op code bits*/) != Poco::Net::WebSocket::FRAME_FLAG_FIN) );
    // send reponse
    std::cout << "Webssocket client msg processing send:" << buffer << std::endl;
    ws.sendFrame(buffer, msgSize, Poco::Net::WebSocket::FRAME_TEXT );
  } while ( repetition++ < nbRepetition && n > 0 && ( (flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) != Poco::Net::WebSocket::FRAME_OP_CLOSE) );
  std::cout << "Webssocket client msg processing end" << std::endl;
}

int main(int argc, char *argv[]) {

  Poco::Net::HTTPClientSession s("127.0.0.1", 8080);
//s.setProxy("localhost", srv.port());
  Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, "/mypath");
  Poco::Net::HTTPResponse response;

  // handle websocket handshake
  Poco::Net::WebSocket ws(s, request, response);

  // application part
  char firstMsg[] = "Hello";
  const int nbRepetition = 5;

  // init the msg exchange
  std::cout << "Webssocket client start" << std::endl;
  ws.sendFrame(firstMsg, sizeof(firstMsg), Poco::Net::WebSocket::FRAME_TEXT );

  // then do some echo with the server 
  EchoLoop(nbRepetition, ws);

  return 0;
}
