/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkoyamba <mkoyamba@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 17:27:59 by bade-lee          #+#    #+#             */
/*   Updated: 2023/06/30 12:45:54 by mkoyamba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Request.hpp"

Request::Request(std::string request_message, Server server) {
	if (!request_message.compare(""))
		return ;
	handle_method(request_message);
	handle_path(request_message, server);
	handle_body(request_message);
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
	if (!check_path(_path, server) && _path.find(".py") == std::string::npos && _method.compare("DELETE"))
		_path = "NULL";
	if (request_message.find(".jpg") != std::string::npos
		|| request_message.find(".jpeg") != std::string::npos
		|| request_message.find(".ico") != std::string::npos
		|| request_message.find(".gif") != std::string::npos
		|| request_message.find(".png") != std::string::npos
		|| request_message.find(".txt") != std::string::npos) {
		begin = request_message.find("/");
		end = request_message.find(' ', begin);
		_path = request_message.substr(begin, end - begin);
		_file = true;
		begin = request_message.find('.');
		end = request_message.find(' ', begin);
		_extension = request_message.substr(begin, end - begin);
	}
	else
		_file = false;
	if (!_extension.compare(".jpg"))
		_extension = "image/jpeg";
	if (!_extension.compare(".jpeg"))
		_extension = "image/jpeg";
	if (!_extension.compare(".ico"))
		_extension = "image/x-icon";
	if (!_extension.compare(".gif"))
		_extension = "image/gif";
	if (!_extension.compare(".png"))
		_extension = "image/png";
	if (!_extension.compare(".txt"))
		_extension = "text/plain";
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

void Request::handle_body(std::string request_message)
{
	std::stringstream ss(request_message);
	std::string token;

	while (std::getline(ss, token))
	{
		if (token == "\r")
			break;

		size_t colon_pos = token.find(':');
		if (colon_pos != std::string::npos)
		{
			std::string key = token.substr(0, colon_pos);
			std::string value = token.substr(colon_pos + 1);
			value.erase(0, value.find_first_not_of(" \t"));
			value.erase(value.find_last_not_of(" \t") + 1);

			_header[key] = value;
		}
	}
	size_t begin = request_message.find("boundary=") + 9;
	_header["boundary"] = request_message.substr(begin, request_message.find("\r\n", begin) - begin);
	std::string remaining_string;
	remaining_string += request_message.substr(request_message.find("\r\n", begin) + 2, std::string::npos);
	_body = remaining_string;
}