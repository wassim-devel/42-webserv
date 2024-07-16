
#ifndef WEBSERV_WEBH_HPP
#define WEBSERV_WEBH_HPP

#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <netinet/in.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>

extern unsigned int line_nb;
std::vector<std::string> split(const std::string &str, char delimiter);
std::vector<std::string> split(const std::string &str, const std::string &delimiter);
std::string cpp_itoa(int number);


#endif //WEBSERV_WEBSERV_HPP
