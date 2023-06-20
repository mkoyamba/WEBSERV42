/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkoyamba <mkoyamba@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 11:45:16 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/06/19 12:58:17 by mkoyamba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

# include "Server.hpp"
# include "webserv.hpp"

class Server;

class Socket {
	public:
		Socket() {};
		virtual ~Socket() {};

		int					getServerSocket() { return _id; };
		sockaddr_in			&getServerAddr() { return _server_addr; };
		std::vector<int>	&getClientSockets() { return _client_sockets; };
		Server				*getServer() { return _server; };

		void	setServerSocket(int id) { _id = id; };
		void	setServer(Server *server) { _server = server; };

	private:
		int					_id;
		sockaddr_in			_server_addr;
		std::vector<int>	_client_sockets;
		Server				*_server;
};

#endif