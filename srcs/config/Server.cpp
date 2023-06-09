/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkoyamba <mkoyamba@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 18:09:48 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/06/23 17:25:26 by mkoyamba         ###   ########.fr       */
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
	handle_extensions(server);
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

int	Server::handle_bodySize(std::string size_str) {
	size_t	multi = 1;
	int		value;
	if (size_str.find_first_not_of("0123456789") != std::string::npos &&
		size_str.find_first_not_of("0123456789") != size_str.size() - 1)
		return 0;
	else if (size_str[size_str.size() - 1] != 'M' && size_str[size_str.size() - 1] != 'K')
		return 0;
	else if (size_str[size_str.size() - 1] == 'K') {
		size_str.erase(size_str.size() - 1, std::string::npos);
		multi = 1000;
	}
	else if (size_str[size_str.size() - 1] == 'M') {
		size_str.erase(size_str.size() - 1, std::string::npos);
		multi = 1000000;
	}
	if (size_str.size() > 4)
		return 0;
	std::istringstream(size_str) >> value;
	return value * multi;

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
	_body_size = 1000000;
	begin = server.find("client_max_body_size") + 21;
	if (begin != std::string::npos + 21) {
		end = server.find('\n', begin);
		_body_size =  handle_bodySize(server.substr(begin, end - begin));
		if (_body_size == 0)
			std::cerr << RED << "Wrong body size." << std::endl;
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
	for (int i = 0; i < 3; i++)
		_methods[i] = false;
	if (methods.find("DELETE") != std::string::npos)
		_methods[DELETE] = true;
	if (methods.find("GET") != std::string::npos)
		_methods[GET] = true;
	if (methods.find("POST") != std::string::npos)
		_methods[POST] = true;
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
			if (!host.compare("127.0.0.1"))
				host = "localhost";
			std::istringstream(port_str) >> port;
			pair = std::make_pair(host, port);
			_listen.push_back(pair);
		}
		while (end < current.size() - 1 && current[end] == ' ')
			end++;
		current.erase(0, end);
	}
}

void	Server::handle_extensions(std::string server) {
	size_t						begin;
	size_t						end;
	std::string					current;

	begin = server.find("cgi_extension") + 14;
	if (begin == std::string::npos + 14)
		return ;
	end = server.find('\n', begin);
	current = server.substr(begin, end - begin);
	begin = 0;
	end = current.find(' ', begin);
	while (end != std::string::npos) {
		_cgi_ext.push_back(current.substr(begin, end - begin));
		begin = end + 1;
		end = current.find(' ', begin);
	}
	_cgi_ext.push_back(current.substr(begin, end - begin));
}

int	Server::check_content(void) {
	if (_listen.size() < 1){
		std::cerr << "Error: the server must have at least 1 host:port" << std::endl;
		return 40;
	}
	if (!_name.compare(""))
		_name = "default";
	return 0;
}