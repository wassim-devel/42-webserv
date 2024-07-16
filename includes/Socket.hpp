/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aoizel <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/15 10:41:36 by aoizel            #+#    #+#             */
/*   Updated: 2024/04/08 09:46:53 by aoizel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <iostream>
#include "Client.hpp"
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <cstdlib>
#include "HTTPMessage.hpp"
#include "VirtualServer.hpp"
#include "HTTPMessage.hpp"

class VirtualServer;

class Socket {
	public:
		Socket(const Socket&);
		Socket(const std::string &, const std::string &);
		~Socket();
		const std::string &getHost() const;
		const std::string &getPort() const;
		Socket &operator=(const Socket&);
		void addServer(VirtualServer);
		void setRunning();
		class SocketException: public std::exception
		{
			virtual const char *what() const throw();
		};
		void display();
		const int &getSockFd() const;
		void	parse_request(const std::string &request);
		void	answer_request(const HTTPMessage &http_request, int connfd);
	private:
		bool _running;
		std::map<int, Client> _clients;
		Socket();
		int 	_sockfd;
		std::string _host;
		std::string _port;
		std::vector<VirtualServer> _servers;
};

#endif //WEBSERV_SOCKET_HPP
