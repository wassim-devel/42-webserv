/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aoizel <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/02 08:42:16 by aoizel            #+#    #+#             */
/*   Updated: 2024/04/08 13:17:28 by aoizel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP
#define CLIENT_TIMEOUT 5

#include <ctime>
#include "HTTPMessage.hpp"
#include <string>


class Client
{
	public:
		Client();
		Client(const Client&);
		~Client();
		Client &operator=(const Client&);
		void setFd(int fd);
		int getFd() const;
		const HTTPMessage &getRequest() const;
		bool isTimedOut() const;
		int readRequest();
	private:
		int _connfd;
		HTTPMessage _request;
		time_t _last_activity;
};

#endif
