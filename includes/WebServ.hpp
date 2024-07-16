/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aoizel <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/15 10:37:37 by aoizel            #+#    #+#             */
/*   Updated: 2024/04/08 09:46:09 by aoizel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP
#include "Socket.hpp"
#include <exception>
#include <string>
#include <vector>

class WebServ {
public:
	public:
		WebServ(const std::string &);
		~WebServ();
		void	http_listen();
		void	start();
		void display();
		class WebServException: public std::exception
		{
			public:
				virtual const char *what() const throw();
				WebServException(const std::string &);
				~WebServException() throw();
			private:
				std::string _msg;
		};
	private:
		std::string _msg;
		WebServ &operator=(const WebServ&);
		WebServ();
		WebServ(const WebServ&);
		std::vector<Socket> _sockets;
		std::map<int, Client> _clients;
		int _epollfd;
		struct epoll_event _event;
		struct epoll_event _events[10];
		std::map<int, Socket*> _socket_map;
		std::map<int, Socket*> _socket_connfd_map;
};


#endif //WEBSERV_WEBSERV_HPP
