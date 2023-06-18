/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bade-lee <bade-lee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 18:09:28 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/06/18 18:48:35 by bade-lee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Config.hpp"


Config::Config() {
}

void	remove_unnecessary(std::string &config) {
	size_t begin;
	size_t end;

	begin = config.find('#');
	while (begin != std::string::npos) {
		end = config.find('\n', begin);
		config.erase(begin, end - begin + 1);
		begin = config.find('#');
	}
	begin = config.find('\t');
	while (begin != std::string::npos) {
		config.erase(begin, 1);
		begin = config.find('\t');
	}
	begin = config.find('\n');
	while (begin != std::string::npos) {
		if (config[begin + 1] == '\n')
			config.erase(begin, 1);
		begin = config.find('\n', begin + 1);
	}
}

std::vector<std::string>	split_servers(std::string config) {
	std::vector<std::string>	result;
	std::string	current;
	size_t	begin;
	size_t	end;
	size_t	nb;

	begin = config.find("server");
	while (begin != std::string::npos) {
		end = config.find('{');
		nb = 1;
		while (nb > 0) {
			end++;
			if (!config[end])
				throw std::runtime_error("Error: Scope error.");
			if (config[end] == '{')
				nb++;
			if (config[end] == '}')
				nb--;
		}
		current = config.substr(begin, end - begin + 1);
		config.erase(begin, end - begin + 1);
		result.push_back(current);
		begin = config.find("server");
	}
	return result;
}

void	Config::handle_file(std::string filename) {
	std::ifstream config(filename.c_str());
	std::stringstream stream;
	stream << config.rdbuf();
	std::string	str(stream.str());
	remove_unnecessary(str);
	std::vector<std::string> servers_str;
	servers_str = split_servers(str);
	for (size_t i = 0; i < servers_str.size(); i++) {
		Server server(servers_str[i]);
		_servers.push_back(server);
	}
	check_ports();
}

void	Config::check_ports(void) {
	std::vector<int> ports;
	for (size_t i = 0; i < size(); i++) {
		for (size_t j = 0; j < _servers[i].getListen().size(); j++){
			ports.push_back(_servers[i].getListen()[j].second);
		}
	}
	for (size_t i = 0; i < ports.size(); i++) {
		for (size_t j = 0; j < ports.size(); j++) {
			if (i == ports.size() - 1 && j == ports.size() - 1)
				break;
			if (i == j)
				j++;
			if (ports[i] == ports[j])
				throw std::runtime_error("Error: Ports error.");
		}
	}
}

int	Config::check_content(void) {
	for (size_t i = 0; i < _servers.size(); i++) {
		int check = _servers[i].check_content();
		if (check)
			return check;
	}
	return 0;
}