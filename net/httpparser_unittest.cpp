#ifndef NET_HTTPPARSER_UNITTEST_H_
#define NET_HTTPPARSER_UNITTEST_H_

#include <string>

#include "gtest/gtest.h"
#include "httpparser.h"

TEST(HTTPPaser, Parse) {
  std::string request = "GET is/dis.html HTTP/1.1\n";

  net::HTTPParser parser;
  net::ParserState state = parser.Parse(request);

  ASSERT_TRUE(state != net::ParserState::ERROR);

  EXPECT_EQ(state, net::ParserState::PARSED);
  EXPECT_EQ(net::RequestMethod::GET, parser.GetRequestMethod());

  EXPECT_STREQ("is/dis.html", parser.GetResourceURI().c_str());

  EXPECT_EQ(parser.GetHTTPVersion(), net::HTTPVersion::VER_1_1);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

#endif // NET_HTTPPARSER_UNITTEST_H_
