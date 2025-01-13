#ifndef GLOBAL_TOOL_H
#define GLOBAL_TOOL_H
#include <string>
void initSpdlog();
unsigned char ToHex(unsigned char x);

unsigned char FromHex(unsigned char x);
std::string UrlEncode(const std::string &str);
#endif