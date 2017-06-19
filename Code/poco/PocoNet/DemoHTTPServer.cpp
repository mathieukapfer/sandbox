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
           << "My 1st POCO HTTP Server"
           << "</title></head>"
           << "<body><h1>"
           << _helloMsg
           << "</h1></body></html>";
    }
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
      if (request.getURI() == "/")
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
