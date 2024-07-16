/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aoizel <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/15 11:16:44 by aoizel            #+#    #+#             */
/*   Updated: 2024/04/08 13:41:37 by aoizel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP
#include "defines.hpp"
#include <fstream>
#include <map>
#include "HTTPMessage.hpp"
#include <fstream>
#include <map>
#include <sys/wait.h>
#include <string>
#include <sys/stat.h>
#include <algorithm>
#include <vector>
#define CGI_TIMEOUT 100

class VirtualServer;
class HTTPMessage;

class Location
{
	public:
		Location();
		Location(const VirtualServer &);
		Location(const VirtualServer &, std::ifstream &);
		Location(const Location &);
		Location &operator=(const Location&);
		~Location();
		const std::string &getRoot() const;
		const std::string &getIndex() const;
		const std::string &getRedirect() const;
		bool getAutoindex() const;
		void setOpt(const std::string &, const std::string &);
		void setRoot(const std::string &);
		void setIndex(const std::string &);
		void setRedirect(const std::string &);
		void setAutoindex(const std::string &);
		void setClientMaxBodySize(const std::string &);
		void setErrorPage(const std::string &);
		void setAllowedMethods(const std::string &);
		void setCGI(const std::string &);
		void setSetCookie(const std::string &);
		void setCookie(const std::string &);
		int answer_request(const HTTPMessage &, int);
		void handleCGI(HTTPMessage &http_response, const std::string &full_path, const HTTPMessage &http_request, const std::string &path_cgi, const std::string &cgi);
		std::string get_full_path(const HTTPMessage &, bool &, const std::string &);
		class LocationException: public std::exception
		{
			public:
				virtual const char *what() const throw();
				LocationException(const std::string &);
				LocationException(const std::string &, int);
				~LocationException() throw();
			private:
				std::string _msg;
		};
		const static std::string optNames[OPTNB];
		static void (Location::*optSetters[OPTNB])(const std::string &);
		void display() const;
	private:
		std::string _root;
		std::string _index;
		std::string _redirect;
		bool _autoindex;
		unsigned int _client_max_body_size;
		std::map<std::string, std::string> _error_pages;
		std::map<std::string, bool> _allowed_methods;
		std::map<std::string, std::string> _cgi;
		std::vector<std::string> _set_cookie;
		std::map<std::string, std::string> _cookie;
};

#endif
