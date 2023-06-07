/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkoyamba <mkoyamba@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 17:09:10 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/06/07 20:10:22 by mkoyamba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"

void	print_listens(Config &config, int server) {
	std::vector<std::pair<std::string, int> >	listen = config.getServer(server).getListen();

	for (size_t i = 0; i < listen.size(); i++) {
		std::cout << "\t\tHost {" << listen[i].first << "} Port {" << listen[i].second << '}' <<std::endl;
	}
}

void	print_methods(Config &config, int server) {
	if (config.getServer(server).getMethod(DELETE))
		std::cout << "DELETE ";
	if (config.getServer(server).getMethod(GET))
		std::cout << "GET ";
	if (config.getServer(server).getMethod(POST))
		std::cout << "POST ";
	if (config.getServer(server).getMethod(PUT))
		std::cout << "PUT ";
	std::cout << std::endl;
}

void	print_errors(Config &config, int server) {
	for (int i = 0; i < 999; i++) {
		if (config.getServer(server).getErrorPage(i).compare(""))
		std::cout << "\t\t" << i << " -> " << config.getServer(server).getErrorPage(i) << std::endl;
	}
}

void	print_locations(Config &config, int server) {
	std::map<std::string, Location>	locations = config.getServer(server).getLocations();
	std::map<std::string, Location>::iterator it;
	for (it = locations.begin(); it != locations.end(); ++it) {
		std::cout << "\t\t" <<it->first << " :" << std::endl;
		if (it->second.getRoot().compare(""))
			std::cout << "\t\t\tRoot : " << it->second.getRoot() << std::endl;
		if (it->second.getIndex().compare(""))
			std::cout << "\t\t\tIndex : " << it->second.getIndex() << std::endl;
		if (it->second.getRedirect().compare(""))
			std::cout << "\t\t\tRedirect : " << it->second.getRedirect() << std::endl;
		if (it->second.getUploadPath().compare(""))
			std::cout << "\t\t\tUpload Path : " << it->second.getUploadPath() << std::endl;
		if (it->second.getCgiPath().compare(""))
			std::cout << "\t\t\tCGI Path : " << it->second.getCgiPath() << std::endl;
		if (it->second.getBodySize().compare(""))
			std::cout << "\t\t\tClient max body size : " << it->second.getBodySize() << std::endl;
		if (it->second.getAutoIndex() == true)
			std::cout << "\t\t\tAutoindex status : on" << std::endl;
		if (it->second.getUpload() == true)
			std::cout << "\t\t\tUpload status : on" << std::endl;
		std::cout << std::endl;
	}
}		

void	print_test(Config &config) {
	std::cout << "Nombre de serveurs : " << config.size() << std::endl;
	std::cout << "\n=============================================\n" << std::endl;
	for (size_t i = 0; i < config.size(); i++) {
		std::cout << "SERVEUR " << i + 1 << " :" << std::endl;
		std::cout << "\tServer name : " << config.getServer(i).getName() << std::endl;
		std::cout << "\tRoot : " << config.getServer(i).getRoot() << std::endl;
		std::cout << "\tIndex : " << config.getServer(i).getIndex() << std::endl;
		std::cout << "\tRedirect : " << config.getServer(i).getRedirect() << std::endl;
		std::cout << "\tUpload path : " << config.getServer(i).getUploadPath() << std::endl;
		std::cout << "\tCGI path : " << config.getServer(i).getCgiPath() << std::endl;
		std::cout << "\tClient max body size : " << config.getServer(i).getBodySize() << std::endl;
		std::cout << "\tAutoindex status : " << config.getServer(i).getAutoIndex() << std::endl;
		std::cout << "\tUpload status : " << config.getServer(i).getUpload() << std::endl;
		std::cout << "\tMethods : ";
		print_methods(config, i);
		std::cout << "\tListens :" << std::endl;
		print_listens(config, i);
		std::cout << "\tError pages :" << std::endl;
		print_errors(config, i);
		std::cout << std::endl;
		std::cout << "\t[LOCATIONS]\n" << std::endl;
		print_locations(config, i);
		std::cout << "\n\n" << std::endl;
	}
}

int	main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "usage: ./webserv [configuration file]" << std::endl;
		return 1;
	}
	std::string filename(argv[1]);
	Config config(filename);
	print_test(config);
}