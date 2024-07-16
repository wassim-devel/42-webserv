/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aoizel <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/21 11:21:22 by aoizel            #+#    #+#             */
/*   Updated: 2024/04/08 13:22:07 by aoizel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/WebServ.hpp"
#include <csignal>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>

unsigned int line_nb = 1;
int sig = 0;

WebServ::WebServ() :  _epollfd(-1)
{
}

static std::vector<Socket>::iterator findSocket(std::vector<Socket>::iterator begin, std::vector<Socket>::iterator end, VirtualServer &vserv)
{
	while (begin != end)
	{
		if (begin->getHost() == vserv.getHost() && begin->getPort() == vserv.getPort())
			break;
		begin++;
	}
	return (begin);
}

WebServ::WebServ(const std::string &config_file) : _epollfd(-1)
{
	std::ifstream config;
	std::string line;


	config.open(config_file.c_str());
	if (!config)
		throw WebServException("Can't open config file");
	while (1)
	{
		std::getline(config, line);
		if (!config)
			break;
		if (line.find_first_not_of(WS) == std::string::npos)
			continue;
		if (line == "server {")
		{
			VirtualServer vserv(config);
			if (findSocket(_sockets.begin(), _sockets.end(), vserv) == _sockets.end())
			{
				Socket socket(vserv.getHost(), vserv.getPort());
				socket.addServer(vserv);
				_sockets.push_back(socket);
			}
			else
			{
				findSocket(_sockets.begin(), _sockets.end(), vserv)->addServer(vserv);
			}
		}
		else
			throw WebServException(std::string("Unexpected token in config file"));
	}

	_epollfd = epoll_create1(0);
	if (_epollfd == -1)
		throw std::runtime_error("epoll create issue");
	memset(&_event, 0, sizeof(_event));
	for (std::vector<Socket>::iterator it = _sockets.begin(); it != _sockets.end(); it++)
	{
		_event.events = EPOLLIN;
		_event.data.fd = it->getSockFd();
		if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, it->getSockFd(), &_event))
		{
			close(_epollfd);
			throw std::runtime_error("epoll ctl issue");
		}
		_socket_map[it->getSockFd()] = &(*it);
		it->setRunning();
	}
}

int	accept_connection(int sockfd)
{
	int connfd;

	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(sockaddr_in);

	connfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
	if (connfd < 0)
	{
		std::cerr << "accept error happened " << std::endl;
		return -1;
	}
	return connfd;
}

bool	is_sock_event(const std::map<int, Socket *> &sockets, int fd)
{
	try
	{
		sockets.at(fd);
		return (true);
	}
	catch (...)
	{
		return (false);
	}
}

void WebServ::http_listen()
{
	int connfd;
	while (1)
	{
		int fd_amount = epoll_wait(_epollfd, _events, 10, -1);
		if (sig == SIGINT)
			break;
		if (fd_amount == -1)
		{
			throw std::runtime_error("epoll wait issue");
		}
		for (int n = 0; n < fd_amount; n++)
		{
			if (is_sock_event(_socket_map, _events[n].data.fd) == true)
			{
				connfd = accept_connection(_events[n].data.fd);
				if (connfd == -1)
				{
					throw std::runtime_error("accept issue");
				}
				_event.events = EPOLLIN;
				_event.data.fd = connfd;
				if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, connfd, &_event) == -1)
				{
					throw std::runtime_error("epoll ctl issue");
				}
				_socket_connfd_map[connfd] = _socket_map.at(_events[n].data.fd);
				_clients[connfd].setFd(connfd);
			}
			else
			{
				if (_events[n].events & EPOLLIN)
				{
					int return_value = _clients.at(_events[n].data.fd).readRequest();
					if (return_value == -1 || return_value == 0)
					{
						epoll_ctl(_epollfd, EPOLL_CTL_DEL, _events[n].data.fd, 0);
						_clients.erase(_events[n].data.fd);
					}
					else
					{
						_event.events = EPOLLOUT;
						_event.data.fd = _events[n].data.fd;
						if (epoll_ctl(_epollfd, EPOLL_CTL_MOD, _events[n].data.fd, &_event) == -1)
							throw std::runtime_error("epoll ctl issue");
					}
				}
				else if (_events[n].events & EPOLLOUT)
				{
					const HTTPMessage &request = _clients.at(_events[n].data.fd).getRequest();
					_socket_connfd_map.at(_events[n].data.fd)->answer_request(request, _events[n].data.fd);
					_event.events = EPOLLIN;
					_event.data.fd = _events[n].data.fd;
					if (epoll_ctl(_epollfd, EPOLL_CTL_MOD, _events[n].data.fd, &_event) == -1)
						throw std::runtime_error("epoll ctl issue");
				}
			}
		}
		for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end();)
		{
			if (it->second.isTimedOut())
				_clients.erase(it++);
			else
				++it;
		}
	}
}

WebServ::~WebServ()
{
	if (_epollfd != -1)
		close(_epollfd);
}

WebServ::WebServ(const WebServ &other)
{
	(void)other;
}

WebServ &WebServ::operator=(const WebServ &other)
{
	(void)other;
	return(*this);
}

void WebServ::display()
{
	for (std::vector<Socket>::iterator it = _sockets.begin(); it != _sockets.end(); it++)
	{
		it->display();
	}
}

WebServ::WebServException::WebServException(const std::string &msg): _msg("Web server error: " + msg)
{
}

const char *WebServ::WebServException::what() const throw()
{
	return (_msg.c_str());
}

WebServ::WebServException::~WebServException() throw()
{
}

void webservSigHandler(int sig_code)
{
	sig = sig_code;
}

void WebServ::start()
{
	struct sigaction	action;

	memset(&action, 0, sizeof(action));
	action.sa_handler = webservSigHandler;
	action.sa_flags = SA_SIGINFO;
	if (sigaction(SIGINT, &action, NULL))
		throw std::runtime_error("Sigaction fail");
	signal(SIGPIPE, SIG_IGN);
	this->http_listen();
	std::cout << std::endl << "Closing WebServ." << std::endl;
}
