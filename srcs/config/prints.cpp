/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prints.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkoyamba <mkoyamba@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/20 11:05:21 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/06/20 12:34:51 by mkoyamba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/webserv.hpp"

void	print_listens(Config &config, int server) {
	std::vector<std::pair<std::string, int> >	listen = config.getServer(server).getListen();

	for (size_t i = 0; i < listen.size(); i++) {
		std::cout << "\t|\tHost {" << listen[i].first << "} Port {" << listen[i].second << '}' <<std::endl;
	}
	std::cout << std::endl;
}

void	print_methods(Config &config, int server) {
	if (config.getServer(server).getMethod(DELETE))
		std::cout << "DELETE ";
	if (config.getServer(server).getMethod(GET))
		std::cout << "GET ";
	if (config.getServer(server).getMethod(POST))
		std::cout << "POST ";
	std::cout << std::endl;
}

void	print_extensions(Config &config, int server) {
	std::vector<std::string>	extensions = config.getServer(server).getExtensions();
	size_t						size = extensions.size();

	for (size_t i = 0; i < size; i++)
		std::cout << extensions[i] << ' ';
	std::cout << std::endl;
}

bool	check_method(std::map<std::string, Location>::iterator it) {
	if (it->second.getMethod(DELETE) || it->second.getMethod(GET)
		|| it->second.getMethod(POST))
		return true;
	return false;
}

void	print_methods_location(std::map<std::string, Location>::iterator it) {
	if (it->second.getMethod(DELETE))
		std::cout << "DELETE ";
	if (it->second.getMethod(GET))
		std::cout << "GET ";
	if (it->second.getMethod(POST))
		std::cout << "POST ";
	std::cout << std::endl;
}

void	print_errors(Config &config, int server) {
	for (int i = 0; i < 999; i++)
		if (config.getServer(server).getErrorPage(i).compare(""))
			std::cout << "\t|\t" << i << " -> " << config.getServer(server).getErrorPage(i) << std::endl;
}

void	print_errors_location(std::map<std::string, Location>::iterator it) {
	int state = 0;
	for (int i = 0; i < 999; i++)
		if (it->second.getErrorPage(i).compare(""))
			state = 1;
	if (state)
		std::cout << "\t|\t|\n\t|\t|\t[Error pages] :" << std::endl;
	else
		return ;
	for (int i = 0; i < 999; i++)
		if (it->second.getErrorPage(i).compare(""))
			std::cout << "\t|\t|\t|\t" << i << " -> " << it->second.getErrorPage(i) << std::endl;
}

void	print_locations(Config &config, int server) {
	std::map<std::string, Location>	locations = config.getServer(server).getLocations();
	std::map<std::string, Location>::iterator it;
	for (it = locations.begin(); it != locations.end(); ++it) {
		std::cout << "\t|\t" << YELLOW << "[" << it->first << "] :" << GREEN << std::endl;
		if (it->second.getRoot().compare(""))
			std::cout << "\t|\t" << YELLOW << "|\t" << NONE << "Root : " << it->second.getRoot() << GREEN << std::endl;
		if (it->second.getIndex().compare(""))
			std::cout << "\t|\t" << YELLOW << "|\t" << NONE << "Index : " << it->second.getIndex() << GREEN << std::endl;
		if (it->second.getRedirect().compare(""))
			std::cout << "\t|\t" << YELLOW << "|\t" << NONE << "Redirect : " << it->second.getRedirect() << GREEN << std::endl;
		if (it->second.getUploadPath().compare(""))
			std::cout << "\t|\t" << YELLOW << "|\t" << NONE << "Upload Path : " << it->second.getUploadPath() << GREEN << std::endl;
		if (it->second.getCgiPath().compare(""))
			std::cout << "\t|\t" << YELLOW << "|\t" << NONE << "CGI Path : " << it->second.getCgiPath() << GREEN << std::endl;
		if (it->second.getBodySize().compare(""))
			std::cout << "\t|\t" << YELLOW << "|\t" << NONE << "Client max body size : " << it->second.getBodySize() << GREEN << std::endl;
		if (it->second.getAutoIndex() == true)
			std::cout << "\t|\t" << YELLOW << "|\t" << NONE << "Autoindex status : on" << GREEN << std::endl;
		if (it->second.getUpload() == true)
			std::cout << "\t|\t" << YELLOW << "|\t" << NONE << "Upload status : on" << GREEN << std::endl;
		if (check_method(it)) {
			std::cout << "\t|\t" << YELLOW << "|\t" << NONE << "Methods : ";
			print_methods_location(it);
			std::cout << GREEN;
		}
		print_errors_location(it);
		std::cout << "\t|\t" << std::endl;
	}
	std::cout << NONE;
}

void	print_servers(Config &config) {
	std::cout << RED << "\n=========================================\n|\t\t\t\t\t|" << std::endl;
	std::cout << "|\tNombre de serveurs : " << config.serverSize() << "\t\t|" << std::endl;
	std::cout << "|\t\t\t\t\t|\n=========================================\n" << NONE << std::endl;
	for (size_t i = 0; i < config.serverSize(); i++) {
		std::cout << RED << "SERVEUR " << i + 1 << " :\n" << NONE << std::endl;
		std::cout << BLUE << "\t==============================" << std::endl;
		std::cout << "\t║ Server name : " << config.getServer(i).getName() << std::endl;
		std::cout << "\t║ Root : " << config.getServer(i).getRoot() << std::endl;
		std::cout << "\t║ Index : " << config.getServer(i).getIndex() << std::endl;
		std::cout << "\t║ Redirect : " << config.getServer(i).getRedirect() << std::endl;
		std::cout << "\t║ Upload path : " << config.getServer(i).getUploadPath() << std::endl;
		std::cout << "\t║ CGI path : " << config.getServer(i).getCgiPath() << std::endl;
		std::cout << "\t║ Client max body size : " << config.getServer(i).getBodySize() << std::endl;
		std::cout << "\t║ Autoindex status : " << config.getServer(i).getAutoIndex() << std::endl;
		std::cout << "\t║ Upload status : " << config.getServer(i).getUpload() << std::endl;
		std::cout << "\t║ Methods : ";
		print_methods(config, i);
		std::cout << "\t║ Extensions : ";
		print_extensions(config, i);
		std::cout << "\t==============================" << NONE << std::endl;
		std::cout << GREEN << "\n\t[Listens] :" << std::endl;
		print_listens(config, i);
		std::cout << "\t[Error pages] :" << std::endl;
		print_errors(config, i);
		std::cout << std::endl;
		std::cout << "\t[LOCATIONS] :\n\t|" << std::endl;
		print_locations(config, i);
		std::cout << "\n" << std::endl;
	}
}

void	print_request(Request request) {
	std::cout << BLUE << "\n[REQUEST]" << std::endl;
	if (request.getMethod().compare(""))
		std::cout << "|\tMethod:\t\t" << request.getMethod() << std::endl;
	if (request.getPath().compare(""))
		std::cout << "|\tPath:\t\t" << request.getPath() << std::endl;
	if (request.getHeader()["Host"].compare(""))
		std::cout << "|\tHost:\t\t" << request.getHeader()["Host"] << std::endl;
	if (request.getHeader()["Connection"].compare(""))
		std::cout << "|\tConnection:\t" << request.getHeader()["Connection"] << std::endl;
	if (request.getHeader()["Referer"].compare(""))
		std::cout << "|\tReferer:\t" << request.getHeader()["Referer"] << std::endl;
	if (request.getHeader()["Origin"].compare(""))
		std::cout << "|\tOrigin:\t\t" << request.getHeader()["Origin"] << std::endl;
	if (request.getHeader()["Content-Type"].compare(""))
		std::cout << "|\tContent-Type:\t" << request.getHeader()["Content-Type"].substr(0, request.getHeader()["Content-Type"].find(";")) << std::endl;
	if (request.getHeader()["Content-Length"].compare(""))
		std::cout << "|\tContent-Length:\t" << request.getHeader()["Content-Length"] << std::endl;
	std::cout << NONE << std::endl;
}

void	print_response(Request response, std::string str) {
	std::string	code;
	std::string	msg;
	int begin;

	begin = str.find("HTTP/1.1") + 9;
	code = str.substr(begin, 3);
	begin += 4;
	msg = str.substr(begin, str.find("\n", begin) - begin);
	if (response.getHeader()["File"].find(".html") == std::string::npos)
		std::cout << GREEN << "\n[RESPONSE FILE]" << std::endl;
	else
		std::cout << YELLOW << "\n[RESPONSE HTML]" << std::endl;
	if (code.compare(""))
		std::cout << "|\tCode:\t\t" << code << std::endl;
	if (msg.compare(""))
		std::cout << "|\tMessage:\t" << msg << std::endl;
	if (response.getHeader()["Server"].compare(""))
		std::cout << "|\tServer:\t\t" << response.getHeader()["Server"] << std::endl;
	if (response.getHeader()["File"].compare(""))
		std::cout << "|\tFile:\t\t" << response.getHeader()["File"] << std::endl;
	if (response.getHeader()["Content-Type"].compare(""))
		std::cout << "|\tContent-Type:\t" << response.getHeader()["Content-Type"] << std::endl;
	if (response.getHeader()["Content-Length"].compare(""))
		std::cout << "|\tContent-Length:\t" << response.getHeader()["Content-Length"] << std::endl;
	std::cout << NONE << std::endl;
}