/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defines.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aoizel <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 10:12:23 by aoizel            #+#    #+#             */
/*   Updated: 2024/04/04 08:38:05 by aoizel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINES_HPP
# define DEFINES_HPP
#include <string>
#include <vector>
#define WS " \t\r\n"
#define CRLF "\r\n"
#define OPTNB 15

extern unsigned int line_nb;

std::vector<std::string> split(const std::string &str, char delimiter);
std::vector<std::string> split(const std::string &str, const std::string &delimiter);
std::string cpp_itoa(int number);

#endif
