/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bade-lee <bade-lee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 17:18:49 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/06/09 10:31:09 by bade-lee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "webserv.hpp"

class Server;

class Config {
	public:
		Config(std::string filename);
		~Config() {};

		Server	&getServer(int index) { return _servers[index]; };
		size_t	size(void) { return _servers.size(); };

		int		check_content(void);

	private:
		std::vector<Server>	_servers;
};

#endif