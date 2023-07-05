/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkoyamba <mkoyamba@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 16:13:00 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/07/05 19:19:29 by mkoyamba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/webserv.hpp"

void	finish_request(int fd, int kq, Config &config) {
	struct kevent ev = config.getMap()[fd].events;
	EV_SET(&ev, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_ONESHOT, 0, 0, 0);
	if (kevent(kq, &ev, 1, NULL, 0, NULL) == -1)
		std::cerr << "Error in kevent" << std::endl;
}

void	close_connection(int client_sock, int kq, Config &config) {
	struct kevent ev;
  	EV_SET(&ev, client_sock, EVFILT_READ, EV_DELETE, 0, 0, 0);
  	if (kevent(kq, &ev, 1, NULL, 0, NULL) == -1) {
    	std::cerr << "Error deleting socket from kqueue: " << std::strerror(errno) << std::endl;
  	}
	config.getMap()[client_sock].request = "";
	for (size_t i = 0; i < config.getSockets().size(); i++) {
		if (std::find(config.getSockets()[i].getClientSockets().begin(), config.getSockets()[i].getClientSockets().end(), client_sock) != config.getSockets()[i].getClientSockets().end()) {
			std::cout << PURPLE << "Connection with fd " << client_sock << " ended." << std::endl;
			config.getSockets()[i].getClientSockets().erase(std::find(config.getSockets()[i].getClientSockets().begin(), config.getSockets()[i].getClientSockets().end(), client_sock));
		}
	}
	close(client_sock);
}

void	get_connections(Socket &socket) {
	int new_socket = 0;

	while(new_socket != -1) {
		int addressLen = sizeof(socket.getServerAddr());
		new_socket = accept(socket.getServerSocket(), (struct sockaddr *)&socket.getServerAddr(), (socklen_t *)&addressLen);
		if (new_socket < 0)
			new_socket = -1;
		else
			socket.putClientSocket(new_socket);
	}
}

void	split_event(int fd, Config &config, int filter, int kq) {
	for (size_t i = 0; i < config.getSockets().size(); i++) {
		int old_size = config.getSockets()[i].getClientSockets().size();
		if (config.getSockets()[i].getServerSocket() == fd) {
			get_connections(config.getSockets()[i]);
			for (size_t j = old_size; j < config.getSockets()[i].getClientSockets().size(); j++) {
				struct kevent event;
				EV_SET(&event, config.getSockets()[i].getClientSockets()[j], EVFILT_READ, EV_ADD, 0, 0, 0);
				struct FdInfo temp;
				temp.events = event;
				config.setMap(config.getSockets()[i].getClientSockets()[j], temp);
				if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
					std::cerr << "Error adding socket to kqueue" << std::endl;
			}
		}
		else if (std::find(config.getSockets()[i].getClientSockets().begin(), config.getSockets()[i].getClientSockets().end(), fd) != config.getSockets()[i].getClientSockets().end()) {
			if (filter == EVFILT_READ) {
				config.getMap()[fd].request = read_request(fd);
				if (!config.getMap()[fd].request.compare("")) {
					close_connection(fd, kq, config);
					return ;
				}
				else if (!config.getMap()[fd].request.compare(""))
					return ;
				finish_request(fd, kq, config);
			}
			else if (filter == EVFILT_WRITE) {
				Request request(config.getMap()[fd].request, *config.getSockets()[i].getServer(), config, fd);
				print_request(request);
				handle_request(request, fd, *config.getSockets()[i].getServer(), config);
				if (config.getMap()[fd].close == true)
					close_connection(fd, kq, config);
			}
		}
	}
}
