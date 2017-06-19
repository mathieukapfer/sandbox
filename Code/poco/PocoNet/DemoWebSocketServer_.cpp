#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"

#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPServerParams.h"

#include "Poco/Net/WebSocket.h"

#include <iostream>
#include <string>

class MyHandler: public Poco::Net::HTTPRequestHandler {
 public:
  MyHandler(std::string helloMsg):_helloMsg(helloMsg) {};
  virtual ~MyHandler() {};

  // Define handler
  virtual void handleRequest(
    Poco::Net::HTTPServerRequest& request,
    Poco::Net::HTTPServerResponse& response) {
    {
      response.setChunkedTransferEncoding(true);
      response.setContentType("text/html");
      std::ostream& ostr = response.send();
      ostr << "<html><head><title>"
           << "My 1st Websocket Server"
           << "</title></head>"
           << "<body><h1>"
           << _helloMsg
           << "</h1></body></html>";
    }
  }
 private:
  std::string _helloMsg;
};

class MyWebSocketHandler: public Poco::Net::HTTPRequestHandler {
 public:
  MyWebSocketHandler(std::string helloMsg):_helloMsg(helloMsg) {};
  virtual ~MyWebSocketHandler() {};

  // Define handler
  virtual void handleRequest(
    Poco::Net::HTTPServerRequest& request,
    Poco::Net::HTTPServerResponse& response) {
    // handler websocket hand check
    Poco::Net::WebSocket ws(request, response);

    int flags = 0;
    int n = 0;
    std::cout << "Webserver msg processing start" << std::endl;
    do { //loop for connection handling
      char buffer[4096] = { 0 };
      int msgSize = 0;
      do { //loop for message
        std::cout << "Webserver msg processing waiting" << std::endl;
        n = ws.receiveFrame(buffer, sizeof(buffer), flags);
        std::cout << "Webserver msg processing recv:" << buffer << std::endl;
        msgSize +=n;
      } while ( n > 0 && ( (flags & 0xf0 /* masking op code bits*/) != Poco::Net::WebSocket::FRAME_FLAG_FIN) );
      // send reponse
      std::cout << "Webserver msg processing send:" << buffer << std::endl;
      ws.sendFrame(buffer, msgSize, Poco::Net::WebSocket::FRAME_TEXT );
    } while (  n > 0 && ( (flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) != Poco::Net::WebSocket::FRAME_OP_CLOSE) );
    std::cout << "Webserver msg processing end" << std::endl;
  }
  
 private:
  std::string _helloMsg;
};


class MyHTTPRequestHandlerFactory:
  public Poco::Net::HTTPRequestHandlerFactory {
 public:

  /// Creates a new request handler for the given HTTP request.
  Poco::Net::HTTPRequestHandler* createRequestHandler(
    const Poco::Net::HTTPServerRequest& request)   {
    if (request.find("Upgrade") != request.end() &&
        Poco::icompare(request["Upgrade"], "websocket") == 0)
        return new MyWebSocketHandler("Hello you have opened a websocket session");
      else if (request.getURI() == "/")
        return new MyHandler("Hello you are on the root");
      else {
        return new MyHandler("Hello, you are: " + request.getURI());
      }
  }
};

int main(int argc, char** argv) {

  Poco::Net::HTTPServerParams* pParams =
    new Poco::Net::HTTPServerParams;
  pParams->setMaxQueued(100);
  pParams->setMaxThreads(16);

  // do tcp bind and listen
  Poco::Net::ServerSocket srs(8080);

  // register client handler factory
  Poco::Net::HTTPServer srv
    (new MyHTTPRequestHandlerFactory, srs, pParams);

  // Start TCP server
  srv.start();

  // wait forever
  while (1) { sleep(1); }

  // Stop the TCP Server
  srv.stop();

  return 0;
}
