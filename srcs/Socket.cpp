/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aoizel <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/21 11:21:02 by aoizel            #+#    #+#             */
/*   Updated: 2024/04/08 13:51:03 by aoizel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Socket.hpp"
#include <asm-generic/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>

int	initialize(const std::string &host, int port)
{
	int sockfd;
	struct sockaddr_in serv_addr;
	int opt = 1;

	sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd == -1)
		return 1;

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	if (host.empty())
		serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	else
		serv_addr.sin_addr.s_addr = inet_addr(host.c_str());
	serv_addr.sin_port = htons(port);
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		std::cerr << "bind failed" << std::endl;
		return -1;
	}
	if (listen(sockfd, 10) < 0)
	{
		std::cerr << "listen failed" << std::endl;
		return -1;
	}
	return sockfd;
}

Socket::Socket(const std::string &host, const std::string &port): _running(false), _sockfd(-1), _host(host), _port(port)
{
	_sockfd = initialize(host, atoi(port.c_str()));
	if (_sockfd == -1)
		throw std::runtime_error("sockfd issue");
}

Socket::~Socket()
{
	if (_running)
	{
		if (_sockfd > 0)
			close(_sockfd);
	}
}

Socket::Socket(const Socket &other)
{
	*this = other;
}

Socket &Socket::operator=(const Socket &other)
{
	_running = other._running;
	_host = other._host;
	_port = other._port;
	_sockfd = other._sockfd;
	_servers = other._servers;
	_clients = other._clients;
	return (*this);
}

const int &Socket::getSockFd() const
{
	return _sockfd;
}

const std::string &Socket::getHost() const
{
	return (_host);
}

const std::string &Socket::getPort() const
{
	return (_port);
}

void Socket::addServer(VirtualServer vserv)
{
	_servers.push_back(vserv);
}

void Socket::setRunning()
{
	_running = true;
}

void Socket::display()
{
	std::cout << "----- SOCKET -----" << std::endl;
	std::cout << "Host: " << _host << std::endl;
	std::cout << "Port: " << _port << std::endl;
	std::cout << std::endl;
	for (std::vector<VirtualServer>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		it->display();
		std::cout << std::endl;
	}
}

void	Socket::answer_request(const HTTPMessage &http_request, int connfd)
{
	if (http_request.isBadRequest() == true)
	{
		HTTPMessage http_response;
		http_response.setStatus("400 Bad Request");
		http_response.setBody("<h1>400 Bad Request</h1>");
		http_response.addHeader("Content-Length", cpp_itoa(http_response.getBody().length()));
		ssize_t size_send = send(connfd, http_response.getMessage().c_str(), http_response.getMessage().length(), MSG_CONFIRM);
		if (size_send <= 0)
			_clients.erase(connfd);
		return ;
	}
	std::string server_name;
	try {
		server_name = http_request.getHeaders().at("Host")[0];
	} catch (...) {
	}
	std::vector<VirtualServer>::iterator it;
	for (it = _servers.end(); it != _servers.begin(); )
	{
		--it;
		if (server_name == it->getServerName())
			break;
	}
	if (it->answer_request(http_request, connfd) <= 0)
		_clients.erase(connfd);
}
