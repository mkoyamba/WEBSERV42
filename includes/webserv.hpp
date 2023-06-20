/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkoyamba <mkoyamba@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 17:10:06 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/06/20 12:43:08 by mkoyamba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream>
# include <fstream>
# include <sstream>
# include <vector>
# include <map>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <sys/event.h>
# include <fcntl.h>

# include "Location.hpp"
# include "Server.hpp"
# include "Config.hpp"
# include "Request.hpp"

# define DELETE	0
# define GET	1
# define POST	2

#define NONE	"\033[m"
#define PURPLE	"\033[0;35m"
#define BLUE	"\033[0;34m"
#define YELLOW	"\033[0;33m"
#define GREEN	"\033[0;32m"
#define RED		"\033[0;31m"
#define GREY	"\033[0;30m"
#define PURPLE_B	"\033[1;35m"

class Config;
class Request;

int		split_servers(Config &config);
int		exec_loop(Server server, std::pair<std::string, int> listen_pair);
void	print_servers(Config &config);
void	print_request(Request request);
void	print_response(Request response, std::string str);

#endif