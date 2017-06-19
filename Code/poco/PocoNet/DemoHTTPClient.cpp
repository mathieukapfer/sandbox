
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"

#include "Poco/StreamCopier.h"

#include <iostream>

int main(int argc, char *argv[]) {

  Poco::Net::HTTPClientSession s("127.0.0.1", 8080);
//s.setProxy("localhost", srv.port());
  Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, "/mypath");
  s.sendRequest(request);

  Poco::Net::HTTPResponse response;
  std::istream& rs = s.receiveResponse(response);
  Poco::StreamCopier::copyStream(rs, std::cout);
  return 0;
}
