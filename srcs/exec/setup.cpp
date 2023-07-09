/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkoyamba <mkoyamba@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 16:04:01 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/07/09 16:21:32 by mkoyamba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/webserv.hpp"

int	server_socket(std::pair<std::string, int> listen_pair, sockaddr_in &sockaddr) {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		std::cerr << "Failed to create socket" << std::endl;
		return 1;
	}
	bzero(&sockaddr, sizeof(sockaddr));
	int	r = fcntl(sockfd, F_SETFL, O_NONBLOCK);
	if (r < 0)
	{
		close(sockfd);
		throw std::runtime_error("fcntl() failed");
	}
	int on = 1;
	r = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
  	if (r < 0)
  	{
		close(sockfd);
		throw std::runtime_error("setsockopt() failed");
  	}

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddr.sin_port = htons(listen_pair.second);

	if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
		std::cerr << RED << "\n\nFailed to bind to port " << listen_pair.second << std::endl;
		close(sockfd);
		return -1;
	}
	if (listen(sockfd, 10) < 0) {
		std::cerr << RED << "\n\nFailed to listen on socket." << std::endl;
		close(sockfd);
		return -1;
	}
	std::cout << "{" << listen_pair.second << "} ";
	return sockfd;
}

int	exec_port(int fd, int kq, Config &config) {
	struct kevent result;

	EV_SET(&result, fd, EVFILT_READ, EV_ADD, 0, 0, 0);
	struct FdInfo temp;
	temp.events = result;
	config.setMap(fd, temp);
	if (kevent(kq, &result, 1, nullptr, 0, nullptr) == -1)
		std::cerr << "Error adding socket to kqueue" << std::endl;
	return 0;
}

Socket	exec_server_sock(std::pair<std::string, int> listen, Server &server) {
	Socket	socket;

	socket.setServerSocket(server_socket(listen, socket.getServerAddr()));
	if(socket.getServerSocket() == -1)
		throw std::runtime_error("Error setting up server socket");
	socket.setServer(&server);
	return socket;
}

int	fill_sockets(Config &config, int kq) {
	
	size_t	size = config.serverSize();

	for (size_t i = 0; i < size; i++) {
		std::vector<std::pair<std::string, int> >	listens;
		listens = config.getServer(i).getListen();
		for (size_t j = 0; j < listens.size(); j++) {
			Socket socket;
			try { socket = exec_server_sock(listens[j], config.getServer(i)); }
			catch (std::exception &e) { throw e; };
			config.getSockets().push_back(socket);
			exec_port(config.getSockets().back().getServerSocket(), kq, config);
		}
	}
	std::cout << NONE << std::endl;
	return 0;
}

int	split_servers(Config &config) {
	int	kq;

	kq = kqueue();
	if (kq == -1)
		return 1;
	try { fill_sockets(config, kq); }
	catch (std::exception &e) { throw e; };
	while (true) {
		struct kevent	evlist[1024];
		int	nev = kevent(kq, NULL, 0, evlist, 1024, NULL);
		if (nev == -1)
			return 1;
		else if (nev > 0) {
			for (int i = 0; i < nev; i++) {
				int fd = evlist[i].ident;
				int filter = evlist[i].filter;
				if (filter == EVFILT_READ || filter == EVFILT_WRITE)
					split_event(fd, config, filter, kq);
			}
		}
	}
	return 0;
}