/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServer.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aoizel <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/15 11:00:50 by aoizel            #+#    #+#             */
/*   Updated: 2024/04/08 09:42:30 by aoizel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VIRTUALSERVER_HPP
# define VIRTUALSERVER_HPP

#include "Location.hpp"
#include "defines.hpp"
#include "HTTPMessage.hpp"
#include <exception>
#include <fstream>
#include <map>
#include <string>
#include <asm-generic/errno-base.h>
#include <cerrno>
#include <cstdlib>
#include <iostream>
#include <sstream>

class Location;

class VirtualServer {
	public:
		VirtualServer();
		VirtualServer(std::ifstream &);
		VirtualServer(const VirtualServer&);
		VirtualServer &operator=(const VirtualServer&);
		~VirtualServer();
		const std::string &getHost() const;
		const std::string &getPort() const;
		const std::string &getServerName() const;
		const std::string &getRoot() const;
		const std::string &getIndex() const;
		bool getAutoindex() const;
		unsigned int getClientMaxBodySize() const;
		std::map<std::string, std::string> getErrorPages() const;
		void setOpt(const std::string &, const std::string &);
		void setHost(const std::string &);
		void setPort(const std::string &);
		void setServerName(const std::string &);
		void setRoot(const std::string &);
		void setIndex(const std::string &);
		void setAutoindex(const std::string &);
		void setErrorPage(const std::string &);
		void setClientMaxBodySize(const std::string &);
		void addLocation(std::string &, Location &);
		class VirtualServerException: public std::exception
		{
		public:
			virtual const char *what() const throw();
			VirtualServerException(const std::string &);
			VirtualServerException(const std::string &, int);
			~VirtualServerException() throw();
		private:
			std::string _msg;
		};
		const static std::string optNames[OPTNB];
		static void (VirtualServer::*optSetters[OPTNB])(const std::string &);
		void display() const;
		std::string	get_full_path(const HTTPMessage &http_request);
		int answer_request(const HTTPMessage &http_request, int connfd);
		std::string get_full_path(const HTTPMessage &, bool &);
	private:
		std::string _host;
		std::string _port;
		std::string _server_name;
		std::string _root;
		std::string _index;
		bool _isindexadded;
		bool _autoindex;
		unsigned int _client_max_body_size;
		std::map<std::string, std::string> _error_pages;
		std::map<std::string, Location> _locations;
		std::string _msg;
};

#endif //WEBSERV_SERVER_HPP
