/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkoyamba <mkoyamba@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 18:09:48 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/06/07 20:05:02 by mkoyamba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"

Server::Server(std::string server) {
	handle_locations(server);
	handle_strings(server);
	handle_boolean(server);
	handle_errors(server);
	handle_methods(server);
	handle_listen(server);
}

void	Server::handle_locations(std::string &server) {
	size_t		begin;
	size_t		end;
	std::string	current;
	std::string	path;

	begin = server.find("location");
	while (begin != std::string::npos) {
		end = server.find('}', begin);
		current = server.substr(begin, end - begin + 1);
		server.erase(begin, end - begin + 1);
		begin = current.find('/');
		end = current.find('{');
		path = current.substr(begin, end - begin - 1);
		begin = current.find('{') + 1;
		end = current.find('}');
		Location location(current.substr(begin, end - begin));
		_locations[path] = location;
		begin = server.find("location");
	}
}

void	Server::handle_strings(std::string &server) {
	size_t		begin;
	size_t		end;
	std::string	current;

	begin = server.find("server_name") + 12;
	if (begin != std::string::npos + 12) {
		end = server.find('\n', begin);
		_name = server.substr(begin, end - begin);
	}
	begin = server.find("root") + 5;
	if (begin != std::string::npos + 5) {
		end = server.find('\n', begin);
		_root = server.substr(begin, end - begin);
	}
	begin = server.find("index") + 6;
	if (begin != std::string::npos + 6 && server[begin - 7] != 'o') {
		end = server.find('\n', begin);
		_index = server.substr(begin, end - begin);
	}
	begin = server.find("redirect") + 9;
	if (begin != std::string::npos + 9) {
		end = server.find('\n', begin);
		_redirect = server.substr(begin, end - begin);
	}
	begin = server.find("upload_path") + 12;
	if (begin != std::string::npos + 12) {
		end = server.find('\n', begin);
		_upload_path = server.substr(begin, end - begin);
	}
	begin = server.find("cgi_path") + 9;
	if (begin != std::string::npos + 9) {
		end = server.find('\n', begin);
		_cgi_path = server.substr(begin, end - begin);
	}
	begin = server.find("client_max_body_size") + 21;
	if (begin != std::string::npos + 21) {
		end = server.find('\n', begin);
		_body_size = server.substr(begin, end - begin);
	}
}

void	Server::handle_boolean(std::string &server){
	size_t		begin;
	size_t		end;

	begin = server.find("autoindex") + 10;
	end = server.find('\n', begin);
	_autoindex = false;
	if (!server.substr(begin, end - begin).compare("on"))
		_autoindex = true;
	begin = server.find("upload") + 7;
	end = server.find('\n', begin);
	_upload = false;
	if (!server.substr(begin, end - begin).compare("on"))
		_upload = true;
}

void	Server::handle_errors(std::string server) {
	size_t		begin;
	size_t		end;
	std::string	current;
	std::string	error_str;
	int			error;

	begin = server.find("error_page");
	while (begin != std::string::npos) {
		end = server.find('\n', begin);
		current = server.substr(begin, end - begin);
		server.erase(begin, end - begin + 1);
		begin = 11;
		end = current.find(' ', begin);
		error_str = current.substr(begin, end - begin);
		if (error_str.find_first_not_of("0123456789") == std::string::npos) {
			begin = current.find('/', end);
			end = current.find('\n', begin);
			std::istringstream(error_str) >> error;
			_error_page[error] = current.substr(begin, end - begin);
		}
		begin = server.find("error_page");
	}
}

void	Server::handle_methods(std::string server) {
	size_t		begin;
	size_t		end;
	std::string	methods;

	begin = server.find("allow_methods") + 14;
	if (begin != std::string::npos + 14) {
		end = server.find('\n', begin);
		methods = server.substr(begin, end - begin);
	}
	for (int i = 0; i < 4; i++)
		_methods[i] = false;
	if (methods.find("DELETE") != std::string::npos)
		_methods[DELETE] = true;
	if (methods.find("GET") != std::string::npos)
		_methods[GET] = true;
	if (methods.find("POST") != std::string::npos)
		_methods[POST] = true;
	if (methods.find("PUT") != std::string::npos)
		_methods[PUT] = true;
}

bool	valid_host(std::string host) {
	std::string	check_str;
	int	check;
	size_t	begin = 0;
	size_t	end = 0;

	if (!host.compare("localhost"))
		return true;
	while (host[begin]) {
		if (host[begin] == '.')
			end++;
		begin++;
	}
	if (end != 3)
		return false;
	begin = 0;
	for (int i = 0; i < 4; i++) {
		end = host.find('.', begin);
		if (end == std::string::npos && i < 3)
			return false;
		check_str = host.substr(begin, end - begin);
		if (check_str.find_first_not_of("0123456789") != std::string::npos)
			return false;
		std::istringstream(check_str) >> check;
		if (check < 0 || check > 255)
			return false;
		begin = end + 1;
	}
	return true;
}

void	Server::handle_listen(std::string server) {
	size_t						begin;
	size_t						end;
	std::string					current;
	std::string					pair_str;
	std::string					host;
	std::string					port_str;
	int							port;
	std::pair<std::string, int>	pair;

	begin = server.find("listen") + 7;
	end = server.find('\n', begin);
	current = server.substr(begin, end - begin);
	while (current.find(':') != std::string::npos) {
		end = current.find(':');
		host = current.substr(0, end);
		begin = end + 1;
		end = current.find(' ', begin);
		if (end == std::string::npos)
			end = current.find('\n', begin);
		port_str = current.substr(begin, end - begin);
		if (valid_host(host) && port_str.find_first_not_of("0123456789") == std::string::npos && port_str.size() < 5) {
			std::istringstream(port_str) >> port;
			pair = std::make_pair(host, port);
			_listen.push_back(pair);
		}
		while (current[end] == ' ')
			end++;
		current.erase(0, end);
	}
}