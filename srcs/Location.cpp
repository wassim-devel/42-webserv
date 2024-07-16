/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aoizel <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/15 15:19:15 by aoizel            #+#    #+#             */
/*   Updated: 2024/04/08 13:43:47 by aoizel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Location.hpp"
#include "../includes/VirtualServer.hpp"
#include <bits/types/struct_timeval.h>
#include <cstdlib>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>

const std::string Location::optNames[OPTNB]
		= {"root", "index", "autoindex", "return", "client_max_body_size",
		   "error_page", "allow", "cgi", "set_cookie", "cookie"};

void (Location::*Location::optSetters[OPTNB])(const std::string &) =
		{&Location::setRoot, &Location::setIndex, &Location::setAutoindex,
		 &Location::setRedirect, &Location::setClientMaxBodySize,
		 &Location::setErrorPage, &Location::setAllowedMethods,
		 &Location::setCGI, &Location::setSetCookie, &Location::setCookie};

Location::Location()
{
}

Location::Location(const VirtualServer &vserv):
	_root(vserv.getRoot()), _index(vserv.getIndex()), _redirect(),
	_autoindex(vserv.getAutoindex()), _client_max_body_size(vserv.getClientMaxBodySize()),
	_error_pages(vserv.getErrorPages())
{
	_allowed_methods["GET"] = true;
	_allowed_methods["POST"] = true;
	_allowed_methods["DELETE"] = true;
}

Location::Location(const VirtualServer &vserv, std::ifstream &config):
	_root(vserv.getRoot()), _index(vserv.getIndex()), _redirect(),
	_autoindex(vserv.getAutoindex()), _client_max_body_size(vserv.getClientMaxBodySize()),
	_error_pages(vserv.getErrorPages())
{
	_allowed_methods["GET"] = true;
	_allowed_methods["POST"] = true;
	_allowed_methods["DELETE"] = true;

	std::string line;
	std::string opt_name;
	std::string opt_value;
	while (1)
	{
		line_nb++;
		std::getline(config, line);
		if (!config)
			throw LocationException("unexpected end of file.");
		line.erase(0, line.find_first_not_of(WS));
		if (line == "}")
			break;
		if (line.find_first_not_of(WS) == std::string::npos)
			continue;
		if (line.find(" ") == std::string::npos || line.find(" ") == line.size() - 1)
			throw LocationException("format error.");
		opt_name = line.substr(0, line.find(" "));
		opt_value = line.substr(line.find(" ") + 1);
		setOpt(opt_name, opt_value);
	}
}

Location::~Location()
{
}

Location::Location(const Location &other)
{
	*this = other;
}

Location &Location::operator=(const Location &other)
{
	if (this == &other)
		return (*this);
	_root = other._root;
	_index = other._index;
	_redirect = other._redirect;
	_autoindex = other._autoindex;
	_allowed_methods = other._allowed_methods;
	_client_max_body_size = other._client_max_body_size;
	_error_pages = other._error_pages;
	_allowed_methods = other._allowed_methods;
	_cgi = other._cgi;
	_set_cookie = other._set_cookie;
	_cookie = other._cookie;
	return(*this);
}

void Location::setRoot(const std::string &opt_value)
{
	_root = opt_value;
}

void Location::setIndex(const std::string &opt_value)
{
	_index = opt_value;
}

void Location::setAutoindex(const std::string &opt_value)
{
	if (opt_value == "on")
		_autoindex = true;
	else if (opt_value == "off")
		_autoindex = false;
	else
		throw LocationException("wrong option value.");
}

void Location::setRedirect(const std::string &opt_value)
{
	_redirect = opt_value;
}

void Location::setClientMaxBodySize(const std::string &opt_value)
{
	long value;
	char *endptr;

	errno = 0;
	value = strtol(opt_value.c_str(), &endptr, 0);
	if (value <= 0 || errno == ERANGE || *endptr != '\0' )
		throw LocationException("wrong value for client_max_body_size");
	_client_max_body_size = value;
}

void Location::setErrorPage(const std::string &opt_value)
{
	if (opt_value.find(" ") == std::string::npos)
		throw LocationException("wrong value for error_page");
	_error_pages[opt_value.substr(0, opt_value.find(" "))] = opt_value.substr(opt_value.find(" ") + 1, std::string::npos);
}

void Location::setAllowedMethods(const std::string &opt_value)
{
	_allowed_methods["GET"] = false;
	_allowed_methods["POST"] = false;
	_allowed_methods["DELETE"] = false;
	std::vector<std::string> methods = split(opt_value, ',');
	for (std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); it++)
	{
		if (*it == "GET" || *it == "POST" || *it == "DELETE")
			_allowed_methods[*it] = true;
		else
	  		throw LocationException("wrong method in allow");
	}
}

void Location::setSetCookie(const std::string &opt_value)
{
	_set_cookie.push_back(opt_value);
}

void Location::setCGI(const std::string &opt_value)
{
	if (opt_value.find(" ") == std::string::npos)
		throw LocationException("wrong value for cgi");
	_cgi[opt_value.substr(0, opt_value.find(" "))] = opt_value.substr(opt_value.find(" ") + 1, std::string::npos);
}

void Location::setCookie(const std::string &opt_value)
{
	if (opt_value.find(" ") == std::string::npos)
		throw LocationException("wrong value for cookie");
	_cookie[opt_value.substr(0, opt_value.find(" "))] = opt_value.substr(opt_value.find(" ") + 1, std::string::npos);
}

void Location::setOpt(const std::string &opt_name, const std::string &opt_value)
{
	unsigned int idx = 0;
	while (idx < OPTNB)
	{
		if (opt_name == optNames[idx])
		{
			(this->*optSetters[idx])(opt_value);
			return ;
		}
		idx++;
	}
	throw LocationException("invalid option");	
}

const std::string &Location::getRoot() const
{
	return (_root);
}

const std::string &Location::getIndex() const
{
	return (_index);
}

bool Location::getAutoindex() const
{
	return (_autoindex);
}

const std::string &Location::getRedirect() const
{
	return (_redirect);
}

Location::LocationException::LocationException(const std::string &msg)
{
	std::stringstream tmp;

	tmp << line_nb;
	_msg = ("Location error on line " + tmp.str() + " : " + msg);
}

Location::LocationException::LocationException(const std::string &msg, int line)
{
	std::stringstream tmp;

	tmp << line;
	_msg = ("Location error on line " + tmp.str() + " : " + msg);
}

const char *Location::LocationException::what() const throw()
{
	return (_msg.c_str());
}

Location::LocationException::~LocationException() throw()
{}

void Location::display() const
{
	std::cout << "Root: " << _root << std::endl;
	std::cout << "Index: " << _index << std::endl;
	std::cout << "Autoindex: " << _autoindex << std::endl;
	std::cout << "Redirect: " << _redirect << std::endl;
	std::cout << "Client max body size: " << _client_max_body_size << std::endl;
	std::cout << "Error pages:" << _error_pages.size() << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = _error_pages.begin(); it != _error_pages.end(); it++)
	{
		std::cout << "	" << it->first << " -> " << it->second << std::endl;
	}
	std::cout << "Allowed methods: " << _allowed_methods.size() << std::endl;
	for (std::map<std::string, bool>::const_iterator it = _allowed_methods.begin(); it != _allowed_methods.end(); it++)
	{
		std::cout << "	" << it->first << " : " << it->second << std::endl;
	}
	std::cout << "CGI: " << _cgi.size() << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = _cgi.begin(); it != _cgi.end(); it++)
	{
		std::cout << "	" << it->first << " : " << it->second << std::endl;
	}
	std::cout << "Cookies: " << _cookie.size() << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = _cookie.begin(); it != _cookie.end(); it++)
	{
		std::cout << "	" << it->first << " : " << it->second << std::endl;
	}
	std::cout << "Set Cookies: " << _set_cookie.size() << std::endl;
	for (std::vector<std::string>::const_iterator it = _set_cookie.begin(); it != _set_cookie.end(); it++)
	{
		std::cout << "	" << *it << std::endl;
	}
	std::cout << std::endl;
}

bool	is_directory(const std::string &path)
{
	struct stat statbuf;

	stat(path.c_str(), &statbuf);
	return S_ISDIR(statbuf.st_mode);
}

std::string directory_listing(const std::string &directory, const std::string &directory_w_root)
{
	std::string html_body = "<html><body><ul>";
	DIR *dir = opendir(directory.c_str());
	struct dirent *s_read;

	if (!dir)
	{
		std::cerr << "opendir failed" << std::endl;
		return ("");
	}
	s_read = readdir(dir);
	while (s_read != NULL)
	{
		html_body += "<a href=\"" + directory_w_root;
		html_body += s_read->d_name;
		if (is_directory(directory + "/" + s_read->d_name))
			html_body += "/\">";
		else
			html_body += "\">";
		html_body += "<li>";
		html_body += s_read->d_name;
		html_body += "</li>";
		html_body += "</a>";
		html_body += "\n";
		s_read = readdir(dir);
	}
	html_body += "</ul></body><html>";
	closedir(dir);
	return html_body;
}

int	setResponseErrorBody(HTTPMessage &http_response, const std::string &full_error, const std::string &error_code, const std::map<std::string, std::string> &error_pages)
{
	http_response.setStatus(full_error);
	std::string test;

	try {
		test = error_pages.at(error_code);
	} catch (...) {

	}

	std::ifstream error_file(test.c_str());
	if (error_file && error_file.is_open())
	{
		std::stringstream body_buffer;
		body_buffer << error_file.rdbuf();
		http_response.setBody(body_buffer.str());
		return 0;
	}
	return -1;
}

void Location::handleCGI(HTTPMessage &http_response, const std::string &full_path, const HTTPMessage &http_request, const std::string &path_cgi, const std::string &cgi)
{
	char buffer[2000];
	char *list_buffer[100] = {const_cast<char*>(path_cgi.c_str()), const_cast<char*>(full_path.c_str()), NULL};
	char *list_buffer_py[100] = {const_cast<char*>("/usr/bin/python3"), const_cast<char*>(path_cgi.c_str()), const_cast<char*>(full_path.c_str()), NULL};
	std::string query_string = http_request.getUrlParams();
	std::string env_query = ("QUERY_STRING=" + query_string);
	char *env[] = {
			const_cast<char*>(env_query.c_str()),
			NULL
	};
	int pipefd[2];
	if (pipe(pipefd) == -1)
		throw std::runtime_error("pipe issue");
	pid_t pid = fork();
	if (pid == -1)
		throw std::runtime_error("fork issue");
	if (pid == 0)
	{
		close(pipefd[0]);
		dup2(pipefd[1], 1);
		close(pipefd[1]);
		if (cgi == "php")
			execve(path_cgi.c_str(), list_buffer, env);
		else
			execve("/usr/bin/python3", list_buffer_py, env);
	}
	else
	{
		close(pipefd[1]);
		ssize_t len_read;
		struct timeval tmstart;
		gettimeofday(&tmstart, 0);
		struct timeval tmend;
		while (waitpid(pid, NULL, WUNTRACED | WNOHANG) != pid)
		{
			gettimeofday(&tmend, 0);
			if ((tmend.tv_sec - tmstart.tv_sec) * 1000 + (tmend.tv_usec - tmstart.tv_usec) / 1000 > CGI_TIMEOUT)
			{
				close(pipefd[0]);
				kill(pid, SIGINT);
				if (setResponseErrorBody(http_response, "504 Gateway Timeout", "504", _error_pages) == -1)
					http_response.setBody("<h1>CGI Timeout</h1>");
				http_response.addHeader("Content-Type", "text/html");
				http_response.addHeader("Content-Type", "charset=UTF-8");
				return;
			}
		}
		len_read = read(pipefd[0], buffer, sizeof(buffer) - 1);
		close(pipefd[0]);
		if (len_read == -1)
			throw std::runtime_error("read issue in location.cpp");
		if (len_read == 0)
		{
			kill(pid, SIGINT);
			if (setResponseErrorBody(http_response, "502 Bad Gateway", "502", _error_pages) == -1)
				http_response.setBody("<h1>CGI Failed</h1>");
			http_response.addHeader("Content-Type", "text/html");
			http_response.addHeader("Content-Type", "charset=UTF-8");
			return;
		}
		buffer[len_read] = '\0';
		std::string body = split(buffer, "\r\n")[2];
		http_response.addHeader("Content-Type", "text/html");
		http_response.addHeader("Content-Type", "charset=UTF-8");
		http_response.setBody(body);
	}
}

int Location::answer_request(const HTTPMessage &http_request, int connfd)
{
	bool isindexadded = false;
	std::string temp_index;
	bool iscookieadded = false;
	DIR *dir;
	try {
		for (std::vector<std::string>::const_iterator it = http_request.getHeaders().at("Cookie").begin(); it != http_request.getHeaders().at("Cookie").end(); it++)
		{
			if (*it == "visited=true" && !_cookie["visited=true"].empty())
			{
				temp_index = _cookie["visited=true"];
				iscookieadded = true;
				break ;
			}
		}
		if (iscookieadded == false)
		{
			temp_index = _index;
		}
	} catch (...) {
		temp_index = _index;
	}
	std::string full_path = this->get_full_path(http_request, isindexadded, temp_index);
	std::string body;
	HTTPMessage http_response;
	bool is_allowed_method = false;
	try {
		if (_allowed_methods.at(http_request.getMethod()) == true)
			is_allowed_method = true;
	} catch (...) {
	}
	for (std::vector<std::string>::iterator it = _set_cookie.begin(); it != _set_cookie.end(); it++)
	{
		http_response.addHeader("Set-Cookie", *it);
	}
	if (http_request.getBody().length() > _client_max_body_size)
	{
		if (setResponseErrorBody(http_response, "413 Request Entity Too Large", "413", _error_pages) == -1)
			http_response.setBody("<h1>413 Request Entity Too Large</h1>");
	}
	else if (is_allowed_method == false)
	{
		if (setResponseErrorBody(http_response, "405 Method Not Allowed", "405", _error_pages) == -1)
			http_response.setBody("<h1>405 Method Not Allowed.</h1>");
	}
	else if (!_redirect.empty())
	{
		http_response.setStatus("301 Moved Permanently");
		http_response.addHeader("Location", _redirect);
	}
	else if (http_request.getMethod() == "GET")
	{
		std::ifstream file(full_path.c_str());
		if (file && file.is_open() && !is_directory(full_path))
		{
			size_t index = full_path.find_last_of(".");
			if (index != std::string::npos)
			{
				if (!_cgi["php"].empty() && full_path.substr(index + 1 ) == "php")
					handleCGI(http_response, get_full_path(http_request, isindexadded, temp_index), http_request, _cgi["php"], "php");
				else if (!_cgi["python"].empty() && full_path.substr(index + 1 ) == "py")
					handleCGI(http_response, get_full_path(http_request, isindexadded, temp_index), http_request, _cgi["python"], "py");
				else
				{
					std::stringstream body_buffer;
					body_buffer << file.rdbuf();
					body = body_buffer.str();
					http_response.setBody(body);
				}
			}
			else
			{
				std::stringstream body_buffer;
				body_buffer << file.rdbuf();
				body = body_buffer.str();
				http_response.setBody(body);
			}
		}
		else if (_autoindex && isindexadded == true && (dir = opendir(full_path.substr(0, full_path.find_last_of("/")).c_str())) && dir != NULL)
		{
			closedir(dir);
			body = directory_listing(full_path.substr(0, full_path.find_last_of("/")), http_request.getPath());
			http_response.setBody(body);
		}
		else
		{
			if (setResponseErrorBody(http_response, "404 Not Found", "404", _error_pages) == -1)
				http_response.setBody("<h1>404 Error</h1>");
		}
	}
	else if (http_request.getMethod() == "POST")
	{
		int number = 0;
		std::string filename = http_request.getFileName();
		if (filename.empty())
		{
			while (access(("database/file" + cpp_itoa(number)).c_str(), F_OK) == 0)
				number++;
			std::ofstream file(("database/file" + cpp_itoa(number)).c_str());
			if (file && file.is_open())
			{
				std::stringstream body_buffer;
				body_buffer << http_request.getBody();
				file << body_buffer.str();
				file.close();
			}
		}
		else
		{
			std::ofstream file(("database/" + filename).c_str(), std::ios::binary);
			if (file && file.is_open())
			{
				std::stringstream body_buffer;
				body_buffer << http_request.getBody();
				file << body_buffer.str();
				file.close();
			}
		}
		std::ifstream file(full_path.c_str());
		if (file && file.is_open() && !is_directory(full_path))
		{
			if (!_cgi["php"].empty())
				handleCGI(http_response, get_full_path(http_request, isindexadded, temp_index), http_request, _cgi["php"], "php");
			else if (!_cgi["python"].empty())
				handleCGI(http_response, get_full_path(http_request, isindexadded, temp_index), http_request, _cgi["python"], "py");
			else
			{
				std::stringstream body_buffer;
				body_buffer << file.rdbuf();
				body = body_buffer.str();
				http_response.setBody(body);
			}
		}
	}
	else if (http_request.getMethod() == "DELETE")
	{
		if (remove(("database" + http_request.getPath()).c_str()) != 0)
		{
			if (setResponseErrorBody(http_response, "404 Not Found", "404", _error_pages) == -1)
				http_response.setBody("<h1>404 Not Found</h1>");
		}
		else
			http_response.setStatus("204 No Content");
	}
	http_response.addHeader("Content-Length", cpp_itoa(http_response.getBody().length()));
	ssize_t size_send = send(connfd, http_response.getMessage().c_str(), http_response.getMessage().length(), MSG_CONFIRM);
	return (size_send);
}

std::string Location::get_full_path(const HTTPMessage &http_request, bool &isindexadded, const std::string &index)
{
	std::string full_path(_root.substr(0, _root.length() - 1) + http_request.getPath());
	if (full_path[full_path.length() - 1] == '/')
	{
		full_path += index;
		isindexadded = true;
	}
	return (full_path);
}
