/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkoyamba <mkoyamba@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 17:18:49 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/06/12 13:22:58 by mkoyamba         ###   ########.fr       */
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