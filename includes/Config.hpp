/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkoyamba <mkoyamba@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 17:18:49 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/06/19 12:46:25 by mkoyamba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "webserv.hpp"
# include "Socket.hpp"

class Server;

class Config {
	public:
		Config();
		~Config() {};

		Server				&getServer(int index) { return _servers[index]; };
		size_t				serverSize(void) { return _servers.size(); };
		std::vector<Socket>	&getSockets() { return _sockets; };

		void	handle_file(std::string filename);
		int		check_content(void);
		void	check_ports(void);

	private:
		std::vector<Server>	_servers;
		std::vector<Socket>	_sockets;
};

#endif