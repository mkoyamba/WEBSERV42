/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bade-lee <bade-lee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 17:27:59 by bade-lee          #+#    #+#             */
/*   Updated: 2023/06/09 18:27:07 by bade-lee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Request.hpp"

Request::Request(std::string request_message, Server server) {
	handle_method(request_message);
	handle_path(request_message, server);
}

void	Request::handle_method(std::string request_message) {
	if (request_message.find("DELETE") != std::string::npos)
		_method = "DELETE";
	if (request_message.find("GET") != std::string::npos)
		_method = "GET";
	if (request_message.find("POST") != std::string::npos)
		_method = "POST";
}


void	Request::handle_path(std::string request_message, Server server) {
	size_t		begin;
	size_t		end;

	begin = request_message.find("/");
	if (begin != std::string::npos) {
		end = request_message.find(' ', begin);
		_path = request_message.substr(begin, end - begin);
	}
	if (!check_path(_path, server))
		_path = "NULL";
}

bool	Request::check_path(std::string path, Server server) {
	bool check = false;
	std::map<std::string, Location> loc = server.getLocations();
	for (std::map<std::string, Location>::iterator it = loc.begin(); it != loc.end(); ++it){
		if (!it->first.compare(path))
			check = true;
	}
	return check;
}