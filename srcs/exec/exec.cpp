/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkoyamba <mkoyamba@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/08 11:43:05 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/06/20 13:00:48 by mkoyamba         ###   ########.fr       */
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
		std::cout << RED << "\n\nFailed to bind to port " << listen_pair.second << std::endl;
		return -1;
	}
	if (listen(sockfd, 1) < 0) {
		std::cout << RED << "\n\nFailed to listen on socket." << std::endl;
		return -1;
	}
	std::cout << "{" << listen_pair.second << "} ";
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

void	send_image(char *str, int client_socket, Server server, std::string response) {
	FILE	*picture;
	picture = fopen(str, "r");
	if (!picture) {
		send_image((char *)"assets/default.png", client_socket, server, response);
		return ;
	}
	fseek(picture, 0, SEEK_END);
	int size = ftell(picture);
	std::string	size_str = "Content-Length: " + std::to_string(size) + "\r\n\n";
	std::string	to_print(response + "File: " + std::string(str) + "\n");
	Request responseC(to_print + size_str, server);
	print_response(responseC, to_print + size_str);
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
		send(client_socket, response.c_str(), response.size(), 0);
		file = "assets" + file;
		send_image((char *)file.c_str(), client_socket, server, response);
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
	Request responseC("File: " + file + "\n" + response, server);
	print_response(responseC, response);
	response += content + "\r\n";
	size_t	size = response.size();
	while (size > 0)
		size -= send(client_socket, response.c_str(), response.size(), 0);
}

void	handle_get(Request request, int client_sock, Server server) {
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

void	handle_post(Request request, int client_sock, Server server) {
	(void)request, (void)client_sock, (void)server;
	std::map<std::string, std::string> header = request.getHeader();
	std::map<std::string, std::string>::iterator	it;
}

void	handle_delete(Request request, int client_sock, Server server) {
	(void)request, (void)client_sock, (void)server;
}

void	handle_request(Request request, int client_sock, Server server) {
	if (!request.getMethod().compare("GET"))
		handle_get(request, client_sock, server);
	else if (!request.getMethod().compare("POST"))
		handle_post(request, client_sock, server);
	else if (!request.getMethod().compare("DELETE"))
		handle_delete(request, client_sock, server);
}

int	exec_port(int socket, int kq) {
	struct kevent result;

	EV_SET(&result, socket, EVFILT_READ, EV_ADD, 0, 0, 0);
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

int	read_request(int client_sock, Socket socket) {
	char buffer[4097];
	int j = read(client_sock, buffer, 4096);
	if (j == -1)
		throw std::runtime_error("Error reading connection");
	buffer[j] = '\0';
	std::string request_str(buffer);
	Request request(request_str, *socket.getServer());
	print_request(request);
	handle_request(request, client_sock, *socket.getServer());
	return 0;
}

int	fill_sockets(Config &config, int kq) {
	
	size_t	size = config.serverSize();

	for (size_t i = 0; i < size; i++) {
		std::vector<std::pair<std::string, int> >	listens;
		listens = config.getServer(i).getListen();
		for (size_t j = 0; j < listens.size(); j++) {
			config.getSockets().push_back(exec_server_sock(listens[j], config.getServer(i)));
			exec_port(config.getSockets().back().getServerSocket(), kq);
		}
	}
	std::cout << NONE << std::endl;
	return 0;
}

void	get_connections(Socket &socket) {
	int new_socket = 0;

	while (new_socket != -1)
	{
		int addressLen = sizeof(socket.getServerAddr());
		new_socket = accept(socket.getServerSocket(), (struct sockaddr *)&socket.getServerAddr(), (socklen_t *)&addressLen);
		if (new_socket < 0)
			new_socket = -1;
		else {
			socket.getClientSockets().push_back(new_socket);
			read_request(new_socket, socket);
		}
	}
}

void	split_event(int fd, int filter, Config config) {
	for (size_t i = 0; i < config.getSockets().size(); i++) {
		std::vector<int>	client_sockets = config.getSockets()[i].getClientSockets();
		if (config.getSockets()[i].getServerSocket() == fd) {
			get_connections(config.getSockets()[i]);
			for (size_t j = 0; j < config.getSockets()[i].getClientSockets().size(); j++) {
				close(config.getSockets()[i].getClientSockets()[j]);
			}
		}
		else if (std::find(client_sockets.begin(), client_sockets.end(), fd) != client_sockets.end()) {
			std::cout << "DEBUG" << std::endl;
			(void)filter;
		}
	}
}

int	split_servers(Config &config) {
	std::vector<struct kevent>	events;
	int	kq;

	kq = kqueue();
	if (kq == -1)
		return 1;
	fill_sockets(config, kq);
	while (true) {
		struct kevent	evlist[1024];
		int	nev = kevent(kq, NULL, 0, evlist, 1024, NULL);
		if (nev == -1)
			throw std::runtime_error("Error waiting for events");
		else if (nev > 0) {
			for (int i = 0; i < nev; i++) {
				std::cerr << "HERE" << std::endl;
				int fd = evlist[i].ident;
				int filter = evlist[i].filter;
				if (filter == EVFILT_READ || filter == EVFILT_WRITE) {
					split_event(fd, filter, config);
				}
			}
		}
	}
	return 0;
}