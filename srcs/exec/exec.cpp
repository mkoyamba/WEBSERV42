/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkoyamba <mkoyamba@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/08 11:43:05 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/06/11 16:12:45 by mkoyamba         ###   ########.fr       */
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

void	send_image(char *str, int client_socket) {
	FILE	*picture;
	picture = fopen(str, "r");
	fseek(picture, 0, SEEK_END);
	int size = ftell(picture);
	std::string	size_str = "Content-Length: " + std::to_string(size) + "\r\n\n";
	std::cerr << size_str + "==============================\n" << std::endl;
	send(client_socket, size_str.c_str(), size_str.size(), 0);
	fclose(picture);
	int fd = open(str, O_RDONLY);
	char	buffer[100];
	size_t	ret = read(fd, buffer, 100);
	while (ret > 0) {
		send(client_socket, buffer, ret, 0);
		ret = read(fd, buffer, 100);
	}
	close(fd);
}

void	send_file(std::string file, int client_socket, Server server, std::string code, std::string type) {
	std::string response = "HTTP/1.1 " + code + "\r\n";
	response += "Date: " + daytime() + "\r\n";
	response += "Server: " + server.getName() + "\r\n";
	response += "Content-Type: " + type + "\r\n";
	std::string	content;
	if (!code.compare("202 OK")) {
		std::cerr << "==============================\nRESPONSE : " + response;
		send(client_socket, response.c_str(), response.size(), 0);
		file = "assets" + file;
		send_image((char *)file.c_str(), client_socket);
		return ;
	}
	else if (file.compare("")) {
		std::ifstream content_stream(file.c_str());
		std::stringstream stream;
		stream << content_stream.rdbuf();
		content = stream.str();
	}
	else
		content = "<h1>ERROR 404 - Page not found</h1>";
	response += "Content-Length: " + std::to_string(content.size()) + "\r\n\n";
	response += content + "\r\n";
	std::cerr << "==============================\nRESPONSE : " + response + file + "==============================\n" << std::endl;
	size_t	size = response.size();
	while (size > 0)
		size -= send(client_socket, response.c_str(), response.size(), 0);
}

void	handle_request(Request request, int client_sock, Server server) {
	
	if (!request.getPath().compare("NULL") && !request.getMethod().compare("GET")) {
		send_file(server.getRoot() + server.getErrorPage(404), client_sock, server, "404 Not Found", "text/html");
		return ;
	}
	if (request.getFile() && !request.getMethod().compare("GET")) {
		send_file(request.getPath(), client_sock, server, "202 OK", request.getExtension());
		return ;
	}
	else if (!request.getMethod().compare("GET")) {
		Location	location = server.getLocations()[request.getPath()];
		std::string	index;
		if (location.getRoot().compare(""))
			index += location.getRoot();
		else
			index += server.getRoot();
		if (!location.getMethod(GET) && !server.getMethod(GET)) {
			if (location.getErrorPage(403).compare(""))
				index += location.getErrorPage(405);
			else
				index += server.getErrorPage(405);
			send_file(index, client_sock, server, "405 Method Not Allowed", "text/html");
			return ;
		}
		if (location.getIndex().compare(""))
			index += location.getIndex();
		else
			index += server.getIndex();
		send_file(index, client_sock, server, "200 OK", "text/html");
	}
}

int	client_socket(int server_sock, sockaddr_in &sockaddr, Server server) {
	struct pollfd fds[2];
	memset(fds, 0, sizeof(fds));
	int timeout = 15000;
	fds[0].fd = server_sock;
	int addr_len = sizeof(sockaddr);
	int client_sock = accept(server_sock, (struct sockaddr *)&sockaddr, (socklen_t*)&addr_len);
	int flags = fcntl(server_sock, F_GETFL, 0);
	flags |= O_NONBLOCK;
	fcntl(server_sock, F_SETFL, flags);
	char buffer[1024];
	fds[0].events = POLLIN;
	fds[1].fd = client_sock;
	fds[1].events = POLLIN;
	if(poll(fds, 2, timeout) > 0) {
		int i = read(fds[1].fd, buffer, 1023);
		if (i == -1)
			return 1;
		buffer[i] = '\0';
		std::string request_str(buffer);
		std::cout << request_str << std::endl;
		Request request(request_str, server);
		handle_request(request, client_sock, server);
	}
	close(client_sock);
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