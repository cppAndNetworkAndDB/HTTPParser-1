#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct StringPiece {
    explicit StringPiece(const char* start = NULL, int len = 0) {
        this->start = start;
        length = len;
    }

    std::string ToString() const {
        std::string out;
        out.resize(length);

        for (int i = 0; i < length; i++) {
            out[i] = start[i];
        }

        return out;
    }

    bool operator==(const StringPiece& other) {
        if (other.length != length) {
            return false;
        }

        for (int i = 0; i < length; i++) {
            if (other.start[i] != start[i]) {
                return false;
            }
        }

        return true;
    }

    const char* start;
    int length;
};

enum Status {
    START = 0,
    METHOD,
    URL,
    HTTP,
    HEADER,
    HEADER_NEWLINE,
    FINISHED,
    ERROR
};

char getNextChar(const std::string& request, int currPos) {
    if (currPos >= request.length()) {
        return '\0';
    }

    return request[currPos];
}

int main() {
    std::string request = "GET /docs/index.html HTTP/1.1\r\n"
                          "Host: www.test101.com\r\n"
                          "Accept: image/gif, image/jpeg, */*\r\n"
                          "Accept-Language: en-us\r\n"
                          "Accept-Encoding: gzip, deflate\r\n"
                          "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)\r\n\r\n";

    StringPiece method;
    StringPiece url;
    StringPiece http;

    std::vector<StringPiece> headers;

    char* start = NULL;
    int len = 0;

    Status status = START;
    int currentPos = 0;
    while ((status != ERROR) && (status != FINISHED)) {
        switch (status) {
            case START:
                start = &request[currentPos];
                status = METHOD;
            break;

            case METHOD: {
                char nextChar = getNextChar(request, currentPos);

                switch (nextChar) {
                    case '\0':
                        status = FINISHED;
                    break;

                    case ' ':
                        method.start = start;
                        method.length = len;

                        currentPos++;

                        start = &request[currentPos];
                        len = 0;

                        status = URL;
                    break;

                    default:
                        len++;
                        currentPos++;
                    break;
                }
            }
            break;

            case URL: {
                char nextChar = getNextChar(request, currentPos);

                switch (nextChar) {
                    case '\0':
                        status = FINISHED;
                    break;

                    case ' ':
                        url.start = start;
                        url.length = len;

                        currentPos++;

                        start = &request[currentPos];
                        len = 0;

                        status = HTTP;
                    break;

                    default:
                        len++;
                        currentPos++;
                    break;
                }
            }
            break;

            case HTTP: {
                char nextChar = getNextChar(request, currentPos);

                switch (nextChar) {
                    case '\0':
                        status = FINISHED;
                    break;

                    case '\r':
                        currentPos++;
                    break;

                    case '\n':
                        http.start = start;
                        http.length = len;

                        currentPos++;

                        start = &request[currentPos];
                        len = 0;

                        status = HEADER;
                    break;

                    default:
                        len++;
                        currentPos++;
                    break;
                }
            }
            break;

            case HEADER: {
                char nextChar = getNextChar(request, currentPos);

                switch (nextChar) {
                    case '\0':
                        status = FINISHED;
                    break;

                    case '\r':
                        currentPos++;
                    break;

                    case '\n': {
                        StringPiece header;
                        header.start = start;
                        header.length = len;

                        headers.push_back(header);

                        currentPos++;

                        start = &request[currentPos];
                        len = 0;

                        status = HEADER_NEWLINE;
                    }
                    break;

                    default:
                        len++;
                        currentPos++;
                    break;
                }
            }
            break;

            case HEADER_NEWLINE: {
                char nextChar = getNextChar(request, currentPos);

                switch (nextChar) {
                    case '\r':
                        currentPos++;
                    break;

                    case '\n':
                        status = FINISHED;
                    break;

                    default:
                        status = HEADER;
                    break;
                }
            }
            break;
        
            default:
                status = FINISHED;
            break;
        }
    }

    cout << "Method: \"" << method.ToString() << '"' << endl;
    cout << "URL: \"" << url.ToString() << '"' << endl;
    cout << "HTTP: \"" << http.ToString() << '"' << endl;

    for (const auto& piece : headers) {
        cout << "Header: \"" << piece.ToString() << '"' << endl;
    }

    return 0;
}
