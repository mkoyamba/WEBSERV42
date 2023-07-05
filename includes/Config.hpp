/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkoyamba <mkoyamba@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 17:18:49 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/07/05 14:06:48 by mkoyamba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "webserv.hpp"
# include "Socket.hpp"

class Server;
struct FdInfo
{
	struct kevent 	events;
	bool			close;
	std::string		request;
};

class Config {
	public:
		Config();
		~Config() {};

		Server					&getServer(int index) { return _servers[index]; };
		std::vector<Server>		&getServers() { return _servers; };
		size_t					serverSize(void) { return _servers.size(); };
		std::vector<Socket>		&getSockets() { return _sockets; };
		std::map<int, FdInfo>	&getMap() { return _fd_map; };

		void	handle_file(std::string filename);
		int		check_content(void);
		void	check_ports(void);
		void	setClose(int fd, bool state) { _fd_map[fd].close = state; }
		void	setMap(int fd, FdInfo &info) {
			_fd_map[fd].events.data = info.events.data;
			_fd_map[fd].events.fflags = info.events.fflags;
			_fd_map[fd].events.filter = info.events.filter;
			_fd_map[fd].events.flags = info.events.flags;
			_fd_map[fd].events.ident = info.events.ident;
			_fd_map[fd].events.udata = info.events.udata;
		};

	private:
		std::vector<Server>		_servers;
		std::vector<Socket>		_sockets;
		std::map<int, FdInfo>	_fd_map;
};

#endif