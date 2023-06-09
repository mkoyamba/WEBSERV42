/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bade-lee <bade-lee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 17:10:06 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/06/09 18:12:35 by bade-lee         ###   ########.fr       */
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
# include <poll.h>
# include <fcntl.h>

# include "Location.hpp"
# include "Server.hpp"
# include "Config.hpp"
# include "Request.hpp"

# define DELETE	0
# define GET	1
# define POST	2

class Config;

int	split_servers(Config config);
int	exec_loop(Server server, std::pair<std::string, int> listen_pair);

#endif