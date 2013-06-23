#include "httpparser.h"

#ifdef DEBUG
#include <iostream>
#endif

namespace net {

namespace {

// METHOD space+ URI(except \t \n |space|) space+ HTTP/digit.digit
enum ReqLineParseState {
  BIG_LETTER = 0,
  SPACE,
  NOT_BLANK,
  HTTP,
  MAJOR_VERSION,
  MINOR_VERSION,
  PARSED,
  ERROR
};

// Appends the digit to the number
// NOTE: Doesn't check for overflow
inline int appendDigit(int number, int digit) {
  return number*10 + digit;
}

} // namespace

ParserState HTTPParser::Parse(std::string& request) {
  m_internalState = ParseRequestLine(request);

  if (m_internalState != ParserState::ERROR) {
    m_internalState = ParseHeaders(request);
  }

  return m_internalState;
}

void HTTPParser::Reset() {
  m_internalState = ParserState::NOT_PARSED;
  m_dErrorPos = -1;
  m_dHTTPMajVer = 1;
  m_dHTTPMinVer = 1;
  m_reqMethod = RequestMethod::NUM_OF_SUPPORTED;
  m_resourceURI.clear();
}

ParserState HTTPParser::ParseRequestLine(std::string& request) {
  ReqLineParseState state = ReqLineParseState::BIG_LETTER;

  std::string methodString;

  bool bURIIsParsed = false;

  int currentCharPos = 0;
  char currentChar = request[currentCharPos];

  while (state != ReqLineParseState::PARSED && state != ReqLineParseState::ERROR) {
    switch (state) {
      case BIG_LETTER:
        if (std::isupper(currentChar)) {
          methodString += currentChar;
          currentCharPos++;
        } else if (std::isspace(currentChar)) {
          state = ReqLineParseState::SPACE;
        } else {
          state = ReqLineParseState::ERROR;
        }
      break;

      case SPACE:
        if (std::isspace(currentChar)) {
          currentCharPos++;
        } else if (!std::isblank(currentChar) && !bURIIsParsed) {
          state = ReqLineParseState::NOT_BLANK;
          bURIIsParsed = true;
        } else if (std::isupper(currentChar)) { // Got to HTTP phase
          state = ReqLineParseState::HTTP;
        } else {
          state = ReqLineParseState::ERROR;
        }
      break;

      case NOT_BLANK:
        if (std::isblank(currentChar) && !std::isspace(currentChar)) {
          state = ReqLineParseState::ERROR;
        } else if (std::isspace(currentChar)) {
          state = ReqLineParseState::SPACE;
        } else {
          m_resourceURI += currentChar;
          currentCharPos++;
        }
      break;

      case HTTP:
        if (request.find("HTTP", currentCharPos) == std::string::npos) {
          state = ReqLineParseState::ERROR;
        } else if (currentChar == '/') {
          state = ReqLineParseState::MAJOR_VERSION;
        } else {
          // "HTTP"
          currentCharPos += 4;
          state = ReqLineParseState::MAJOR_VERSION;
        }

        currentCharPos++;
      break;

      case MAJOR_VERSION:
        if (std::isdigit(currentChar)) {
          // Making char into int using ASCII properties
          m_dHTTPMajVer = appendDigit(m_dHTTPMajVer, currentChar - '0');

          currentCharPos++;
        } else if (currentChar == '.') {
          state = ReqLineParseState::MINOR_VERSION;
          currentCharPos++;
        } else {
          state = ReqLineParseState::ERROR;
        }
      break;

      case MINOR_VERSION:
        if (std::isdigit(currentChar)) {
          m_dHTTPMinVer = appendDigit(m_dHTTPMinVer, currentChar - '0');

          currentCharPos++;
        } else if (currentChar == '\n') {
          state = ReqLineParseState::PARSED;
          currentCharPos++;
        } else {
          state = ReqLineParseState::ERROR;
        }
      break;

      default:
      break;
    }

    currentChar = request[currentCharPos];
  }

  if (state != ReqLineParseState::PARSED) {
#ifdef DEBUG
    std::cout << "\tError: An error occured while parsing" << std::endl;
#endif
    m_dErrorPos = currentCharPos;
    return ParserState::ERROR;
  }

  if (methodString == "GET") {
    m_reqMethod = RequestMethod::GET;
  } else if (methodString == "HEAD") {
    m_reqMethod = RequestMethod::HEAD;
  } else {
    // Denotes an error
    m_reqMethod = RequestMethod::NUM_OF_SUPPORTED;

    // An error occured at the beginning of the request
    m_dErrorPos = 0;
    return ParserState::ERROR;
  }

  if (m_dHTTPMajVer == 1) {
    switch (m_dHTTPMinVer) {
      case 1:
        m_httpVer = HTTPVersion::VER_1_1;
      break;

      case 0:
        m_httpVer = HTTPVersion::VER_1_0;
      break;

      default:
        m_httpVer = HTTPVersion::CUSTOM_VER;
      break;
    }
  } else {
    m_httpVer = HTTPVersion::CUSTOM_VER;
  }

  return ParserState::PARSED;
}

ParserState HTTPParser::ParseHeaders(std::string& request) {
  // UNUSED, if you may
  (void)request;

  return ParserState::PARSED;
}


} // namespace net
