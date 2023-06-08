/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkoyamba <mkoyamba@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/08 11:43:05 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/06/08 15:34:04 by mkoyamba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/webserv.hpp"

int	exec_loop(Server server, std::pair<std::string, int> listen_pair) {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
		return 1;
	}
	sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(listen_pair.second);

	if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
		std::cout << "Failed to bind to port " << listen_pair.second << std::endl;
		return 1;
	}
	if (listen(sockfd, 1) < 0) {
		std::cout << "Failed to listen on socket." << std::endl;
		return 1;
	}

	int addrlen = sizeof(sockaddr);
	int connection = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
	if (connection < 0) {
		std::cout << "Failed to grab connection." << std::endl;
		return 1;
	}

	char buffer[2048];
	while (1) {
		int i = read(connection, buffer, 2047);
		buffer[i] = '\0';
		std::string request(buffer);
		if (!request.compare("exit\r\n"))
			break ;
		std::cout << "The message was: " << request << " from port " << listen_pair.second << std::endl;
		std::string response = "Answer from server " + server.getName() + '\n';
		send(connection, response.c_str(), response.size(), 0);
	}
	close(connection);
	close(sockfd);
	return 0;
}

int	split_ports(Server server) {
	size_t	size = server.getListen().size();
	std::vector<pid_t> pids;

	for (size_t i = 0; i < size; i++) {
		pid_t	pid = fork();
		if (pid < 0)
			return 1;
		else if (pid == 0)
			return exec_loop(server, server.getListen()[i]);
		else
			pids.push_back(pid);
	}
	for (size_t i = 0; i < pids.size(); i++)
		waitpid(pids[i], NULL, 0);
	return 0;
}

int	split_servers(Config config) {
	size_t	size = config.size();
	std::vector<pid_t> pids;

	for (size_t i = 0; i < size; i++) {
		pid_t	pid = fork();
		if (pid < 0)
			return 1;
		else if (pid == 0)
			return split_ports(config.getServer(i));
		else
			pids.push_back(pid);
	}
	for (size_t i = 0; i < pids.size(); i++)
		waitpid(pids[i], NULL, 0);
	return 0;
}