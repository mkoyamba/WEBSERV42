/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkoyamba <mkoyamba@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/08 11:43:05 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/06/09 17:00:02 by mkoyamba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/webserv.hpp"

int	server_socket(std::pair<std::string, int> listen_pair, sockaddr_in &sockaddr) {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
		return 1;
	}
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(listen_pair.second);

	if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
		std::cout << "Failed to bind to port " << listen_pair.second << std::endl;
		return -1;
	}
	if (listen(sockfd, 1) < 0) {
		std::cout << "Failed to listen on socket." << std::endl;
		return -1;
	}
	return sockfd;
}

std::string	daytime(void) {
	time_t		rawtime;
	struct tm	*timeinfo;
	char		buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 80, "%a %b %d %H:%M:%S %Y", timeinfo);
	std::string	time(buffer);
	return time;
}

void	send_file(std::string file, int client_socket, Server server) {
	std::string response = "HTTP/1.1 200 OK\r\n";
	response += "Date: " + daytime() + "\r\n";
	response += "Server: " + server.getName() + "\r\n";
	response += "Content-Type: text/html\r\n";

	std::ifstream content_stream(file.c_str());
	std::stringstream stream;
	stream << content_stream.rdbuf();
	std::string	content(stream.str());

	response += "Content-Length: " + std::to_string(content.size()) + "\r\n\n";
	response += content + "\r\n";
	size_t	size = response.size();
	while (size > 0)
		size -= send(client_socket, response.c_str(), response.size(), 0);
}

int	client_socket(int server_sock, sockaddr_in &sockaddr, Server server) {
	struct pollfd fds[2];
	memset(fds, 0, sizeof(fds));
	int timeout = 15000;
	fds[0].fd = server_sock;
	int addr_len = sizeof(sockaddr);
	int client_socket = accept(server_sock, (struct sockaddr *)&sockaddr, (socklen_t*)&addr_len);
	int flags = fcntl(server_sock, F_GETFL, 0);
	flags |= O_NONBLOCK;
	fcntl(server_sock, F_SETFL, flags);
	char buffer[1024];
	fds[0].events = POLLIN;
	fds[1].fd = client_socket;
	fds[1].events = POLLIN;
	if(poll(fds, 2, timeout) > 0) {
		int i = read(fds[1].fd, buffer, 1023);
		if (i == -1)
			return 1;
		buffer[i] = '\0';
		std::string request(buffer);
		std::cout << request << std::endl;
		if (request.substr(0, 4).compare("GET")) {
			send_file("index.html", client_socket, server);
		}
	}
	close(client_socket);
	return 0;
}

int	exec_loop(Server server, std::pair<std::string, int> listen_pair) {
	(void)server;
	sockaddr_in sockaddr;
	int	server_sock = server_socket(listen_pair, sockaddr);
	std::cerr << listen_pair.second << std::endl;
	while (true)
		client_socket(server_sock, sockaddr, server);
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