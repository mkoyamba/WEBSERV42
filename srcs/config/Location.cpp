/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkoyamba <mkoyamba@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 18:09:30 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/06/08 10:32:44 by mkoyamba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Location.hpp"

Location::Location(std::string location) {
	handle_strings(location);
	handle_boolean(location);
	handle_errors(location);
	handle_methods(location);
}

void	Location::handle_strings(std::string &location) {
	size_t		begin;
	size_t		end;
	std::string	current;

	begin = location.find("root") + 5;
	if (begin != std::string::npos + 5) {
		end = location.find('\n', begin);
		_root = location.substr(begin, end - begin);
	}
	begin = location.find("index") + 6;
	if (begin != std::string::npos + 6 && location[begin - 7] != 'o') {
		end = location.find('\n', begin);
		_index = location.substr(begin, end - begin);
	}
	begin = location.find("redirect") + 9;
	if (begin != std::string::npos + 9) {
		end = location.find('\n', begin);
		_redirect = location.substr(begin, end - begin);
	}
	begin = location.find("upload_path") + 12;
	if (begin != std::string::npos + 12) {
		end = location.find('\n', begin);
		_upload_path = location.substr(begin, end - begin);
	}
	begin = location.find("cgi_path") + 9;
	if (begin != std::string::npos + 9) {
		end = location.find('\n', begin);
		_cgi_path = location.substr(begin, end - begin);
	}
	begin = location.find("client_max_body_size") + 21;
	if (begin != std::string::npos + 21) {
		end = location.find('\n', begin);
		_body_size = location.substr(begin, end - begin);
	}
}

void	Location::handle_boolean(std::string &location){
	size_t		begin;
	size_t		end;

	begin = location.find("autoindex") + 10;
	end = location.find('\n', begin);
	_autoindex = false;
	if (!location.substr(begin, end - begin).compare("on"))
		_autoindex = true;
	begin = location.find("upload") + 7;
	end = location.find('\n', begin);
	_upload = false;
	if (!location.substr(begin, end - begin).compare("on"))
		_upload = true;
}

void	Location::handle_errors(std::string location) {
	size_t		begin;
	size_t		end;
	std::string	current;
	std::string	error_str;
	int			error;

	begin = location.find("error_page");
	while (begin != std::string::npos) {
		end = location.find('\n', begin);
		current = location.substr(begin, end - begin);
		location.erase(begin, end - begin + 1);
		begin = 11;
		end = current.find(' ', begin);
		error_str = current.substr(begin, end - begin);
		if (error_str.find_first_not_of("0123456789") == std::string::npos) {
			begin = current.find('/', end);
			end = current.find('\n', begin);
			std::istringstream(error_str) >> error;
			_error_page[error] = current.substr(begin, end - begin);
		}
		begin = location.find("error_page");
	}
}

void	Location::handle_methods(std::string location) {
	size_t		begin;
	size_t		end;
	std::string	methods;

	begin = location.find("allow_methods") + 14;
	if (begin != std::string::npos + 14) {
		end = location.find('\n', begin);
		methods = location.substr(begin, end - begin);
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