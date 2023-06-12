/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkoyamba <mkoyamba@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/08 11:43:05 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/06/12 17:11:42 by mkoyamba         ###   ########.fr       */
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
	std::cout << "Listening to port " << listen_pair.second << std::endl;
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

int	exec_port(int socket, int kq) {
	struct kevent result;

	EV_SET(&result, socket, EVFILT_READ, EV_ADD, 0, 0, 0);
	if (kevent(kq, &result, 1, nullptr, 0, nullptr) == -1)
		throw std::runtime_error("Error adding event to kqueue");
	return 0;
}

int	exec_server_sock(std::pair<std::string, int> listens, std::map<int, std::pair<sockaddr_in, Server> > &pairs, Server server) {
	std::pair<sockaddr_in, Server>	socket_ad;
	socket_ad.second = server;
	int s = server_socket(listens, socket_ad.first);
	if (s == -1)
		return s;
	pairs[s] = socket_ad;
	return s;
}

int	split_servers(Config config) {
	
	size_t	size = config.size();
	std::map<int, std::pair<sockaddr_in, Server> >	pairs;
	std::vector<struct kevent>							events;
	int											kq;

	kq = kqueue();
	if (kq == -1)
		return 1;
	for (size_t i = 0; i < size; i++) {
		std::vector<std::pair<std::string, int> >	listens;
		listens = config.getServer(i).getListen();
		for (size_t j = 0; j < listens.size(); j++) {
			int socket = exec_server_sock(listens[j], pairs, config.getServer(i));
			if (socket == -1)
				return 1;
			exec_port(socket, kq);
		}
	}
	while (true) {
		struct kevent	evlist[1024];
		int	nev = kevent(kq, NULL, 0, evlist, 1024, NULL);
		if (nev == -1)
			throw std::runtime_error("Error waiting for events");
		else if (nev > 0) {
			for (int i = 0; i < nev; i++) {
				int addr_len = sizeof(pairs[evlist[i].ident].first);
				int client_sock = accept(evlist[i].ident, (struct sockaddr *)&pairs[evlist[i].ident].first, (socklen_t*)&addr_len);
				char buffer[4097];
				int j = read(client_sock, buffer, 4096);
				if (j == -1)
					return 1;
				buffer[j] = '\0';
				std::string request_str(buffer);
				std::cout << request_str << std::endl;
				Request request(request_str, pairs[evlist[i].ident].second);
				handle_request(request, client_sock, pairs[evlist[i].ident].second);
			}	
		}
	}
	return 0;
}