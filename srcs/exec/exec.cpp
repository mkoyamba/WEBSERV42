/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkoyamba <mkoyamba@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/08 11:43:05 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/06/30 12:38:00 by mkoyamba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/webserv.hpp"

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
		size -= send(client_socket, response.c_str(), response.size(), 0);;
}

void	redirect(int client_sock, std::string redirection) {
	std::string	response("HTTP/1.1 301 Moved Permanently\r\n");
	response += "Location: ";
	response += redirection;
	response += "\r\n";
	response += "Content-Length: 0\r\n\r\n";
	send(client_sock, response.c_str(), response.size(), 0);
}

void	handle_get(Request request, int client_sock, Server server) {
	if (!request.getPath().compare("NULL") && !request.getMethod().compare("GET")) {
		send_file(server.getRoot() + server.getErrorPage(404), client_sock, server, "404 Not Found", "text/html");
		return ;
	}
	else if (request.getFile() && !request.getMethod().compare("GET")) {
		send_file(request.getPath(), client_sock, server, "202 OK", request.getExtension());
		return ;
	}
	else if (!request.getMethod().compare("GET")) {
		Location	location = server.getLocations()[request.getPath()];
		std::string	index;
		if (location.getRedirect().compare("")) {
			redirect(client_sock, location.getRedirect());
			return ;
		}
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
	std::map<std::string, std::string> header = request.getHeader();
	std::string raw_body = request.getBody();
	size_t begin = raw_body.find("filename=") + 10;
	std::string filename = raw_body.substr(begin, raw_body.find('\"', begin) - begin);
	std::string delim = request.getHeader()["boundary"];
	char buff[100];
	bzero(buff, 100);
	std::string buffer;
	int chars = read(client_sock, buff, 99);
	while (chars > 0) {
		for (int i = 0; i < chars; i++)
			buffer.push_back(buff[i]);
		bzero(buff, 100);
		chars = read(client_sock, buff, 99);
	}
	raw_body += buffer;
	std::string body;
	begin = raw_body.find("\r\n\r\n") + 4;
	begin = raw_body.find("\r\n\r\n", begin) + 4;
	body = raw_body.substr(begin, raw_body.find(delim, begin) - begin - 4);
	if ((int)body.size() > server.getBodySize()) {
		Location	location = server.getLocations()[request.getPath()];
		std::string	index;
		std::string	root = location.getRoot();
		if (!root.compare(""))
			root = server.getRoot();
		root += "/";
		index += root;
		index += location.getErrorPage(413);
		if (!index.compare(root))
			index += server.getErrorPage(413);
		if (!index.compare(root))
			send_file("www/homepage.html", client_sock, server, "413 Content Too Large", "text/html");
		else
			send_file(index, client_sock, server, "413 Content Too Large", "text/html");
		return ;
	}
	std::ofstream file("upload/" + filename);
	file << body;
	file.close();
	send_file("www/upload.html", client_sock, server, "200 OK", "text/html");
}

void	handle_delete(Request request, int client_sock, Server server) {
	std::string path;
	char	*arg[4];
	int 	pip[2];

	std::string	referer = request.getHeader()["Referer"];
	size_t begin = referer.find("localhost") + 9;
	std::string	location_str = referer.substr(begin, std::string::npos);
	Location location = server.getLocations()[location_str];
	path = "upload" + request.getPath();
	std::string root = location.getRoot();
	if (!root.compare(""))
		root = server.getRoot();
	root += "/";
	std::string error = root;
	error += location.getErrorPage(204);
	if (!error.compare(root))
		error += server.getErrorPage(204);
	std::string cgi = server.getCgiPath() + "/delete.py";
	arg[0] = (char *)"delete.py";
	arg[1] = (char *)path.c_str();
	arg[2] = (char *)error.c_str();
	arg[3] = NULL;
	pipe(pip);
	pid_t	pid = fork();
	if (pid < 0)
		return ;
	if (pid == 0) {
		close(pip[0]);
		dup2(pip[1], STDOUT_FILENO);
		execve(cgi.c_str(), arg, NULL);
		exit(0);
	}
	else {
		waitpid(pid, NULL, 0);
		close(pip[1]);
		char buff[1000];
		bzero(buff, 1000);
		read(pip[0], buff, 999);
		send(client_sock, buff, strlen(buff), 0);
		std::string response_str;
		for (size_t i = 0; i < strlen(buff); i++)
			response_str.push_back(buff[i]);
		Request	response(response_str, server);
		print_response(response, response_str);
	}
}

void	handle_request(Request request, int client_sock, Server server) {
	if (!request.getMethod().compare("GET"))
		handle_get(request, client_sock, server);
	else if (!request.getMethod().compare("POST"))
		handle_post(request, client_sock, server);
	else if (!request.getMethod().compare("DELETE"))
		handle_delete(request, client_sock, server);
}

std::string	read_request(int client_sock) {
	std::string request_str = "";
	char buffer[101];
	bzero(buffer, 101);
	int j = read(client_sock, buffer, 100);
	while (j > 0) {
		for (int i = 0; i < j; i++)
			request_str.push_back(buffer[i]);
		bzero(buffer, 101);
		j = read(client_sock, buffer, 100);
	}
	return request_str;
}
