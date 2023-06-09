/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bade-lee <bade-lee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 17:09:10 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/06/09 10:39:46 by bade-lee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"

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
		std::cout << "\t|\t[" <<it->first << "] :" << std::endl;
		if (it->second.getRoot().compare(""))
			std::cout << "\t|\t|\tRoot : " << it->second.getRoot() << std::endl;
		if (it->second.getIndex().compare(""))
			std::cout << "\t|\t|\tIndex : " << it->second.getIndex() << std::endl;
		if (it->second.getRedirect().compare(""))
			std::cout << "\t|\t|\tRedirect : " << it->second.getRedirect() << std::endl;
		if (it->second.getUploadPath().compare(""))
			std::cout << "\t|\t|\tUpload Path : " << it->second.getUploadPath() << std::endl;
		if (it->second.getCgiPath().compare(""))
			std::cout << "\t|\t|\tCGI Path : " << it->second.getCgiPath() << std::endl;
		if (it->second.getBodySize().compare(""))
			std::cout << "\t|\t|\tClient max body size : " << it->second.getBodySize() << std::endl;
		if (it->second.getAutoIndex() == true)
			std::cout << "\t|\t|\tAutoindex status : on" << std::endl;
		if (it->second.getUpload() == true)
			std::cout << "\t|\t|\tUpload status : on" << std::endl;
		if (check_method(it)) {
			std::cout << "\t|\t|\tMethods : ";
			print_methods_location(it);
		}
		print_errors_location(it);
		std::cout << "\t|\t" << std::endl;
	}
}		

void	print_test(Config &config) {
	std::cout << "\n=========================================\n|\t\t\t\t\t|" << std::endl;
	std::cout << "|\tNombre de serveurs : " << config.size() << "\t\t|" << std::endl;
	std::cout << "|\t\t\t\t\t|\n=========================================\n" << std::endl;
	for (size_t i = 0; i < config.size(); i++) {
		std::cout << "SERVEUR " << i + 1 << " :\n" << std::endl;
		std::cout << "\t==============================" << std::endl;
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
		std::cout << "\t==============================" << std::endl;
		std::cout << "\n\t[Listens] :" << std::endl;
		print_listens(config, i);
		std::cout << "\t[Error pages] :" << std::endl;
		print_errors(config, i);
		std::cout << std::endl;
		std::cout << "\t[LOCATIONS] :\n\t|" << std::endl;
		print_locations(config, i);
		std::cout << "\n=====================================================================" << std::endl;
		std::cout << "\n" << std::endl;
	}
}

int	main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "usage: ./webserv [configuration file]" << std::endl;
		return 1;
	}
	std::string filename(argv[1]);
	Config config(filename);
	int check = config.check_content();
	if (check)
		return check;
//	print_test(config);
	return split_servers(config);
	return 1;
}