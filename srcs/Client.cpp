/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aoizel <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/02 09:19:26 by aoizel            #+#    #+#             */
/*   Updated: 2024/04/08 13:35:40 by aoizel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"
#include <ctime>
#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

Client::Client(): _connfd(-1), _last_activity(time(0))
{
}

Client::~Client()
{
	if (_connfd != -1)
		close(_connfd);
}

Client::Client(const Client &other)
{
	*this = other;
}

Client &Client::operator=(const Client &other)
{
	if (this == &other)
		return (*this);
	_connfd = other._connfd;
	_last_activity = other._last_activity;
	return(*this);
}

void Client::setFd(int fd)
{
	_connfd = fd;
}

int Client::getFd() const
{
	return (_connfd);
}

const HTTPMessage &Client::getRequest() const
{
	return (_request);
}

bool Client::isTimedOut() const
{
	return (time(0) - _last_activity >= CLIENT_TIMEOUT);
}

int Client::readRequest()
{
	_last_activity = time(0);
	char buffer[10000];
	ssize_t len_read = recv(_connfd, buffer, sizeof(buffer) - 1, O_NONBLOCK);
	if (len_read <= 0)
		return (len_read);
	buffer[len_read] = '\0';
	_request = HTTPMessage(buffer);
	return (-2);
}
