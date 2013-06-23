#include <iostream>

#include "net/httpparser.h"

using namespace std;

int main() {
  string request = "GET is/dis.html HTTP/1.1\n";
  //string request = "NO /wat/is/dis.html HTTP/10.100\n";

  net::HTTPParser parser;
  net::ParserState retCode = parser.Parse(request);

  if (retCode == net::ParserState::PARSED) {
    std::cout << "Request method is ";
    switch (parser.GetRequestMethod()) {
      case net::RequestMethod::GET:
        cout << "GET" << endl;
      break;

      case net::RequestMethod::HEAD:
        cout << "HEAD" << endl;
      break;

      default:
        cout << "Error request method" << endl;
      break;
    }

    cout << endl << "URI is " << parser.GetResourceURI() << endl;

    cout << endl << "HTTP/" << parser.GetMajHTTPVersion() << '.' << parser.GetMinHTTPVersion() << endl;
  } else {
    cout << "An error occured whilst parsing at " << parser.GetErrorPos() << endl;
  }

  cin.ignore();
  return 0;
}
