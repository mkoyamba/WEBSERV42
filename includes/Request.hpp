/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkoyamba <mkoyamba@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 17:24:45 by bade-lee          #+#    #+#             */
/*   Updated: 2023/07/05 14:09:08 by mkoyamba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "webserv.hpp"

class Config;

class Request {
	public:
		Request(std::string Request_message, Server server, Config &config, int fd);
		~Request() {};

		std::string							getMethod(void) { return _method; };
		std::string							getPath(void) { return _path; };
		std::string							getExtension(void) { return _extension; };
		std::string							getBody(void) { return _body; };
		bool								getFile(void) { return _file; };
		std::map<std::string, std::string>	getHeader(void) { return _header; };
		void								setExtension(std::string ext) { _extension = ext; };

		void		handle_method(std::string request_message);
		void		handle_path(std::string request_message, Server server);
		void		handle_body(std::string request_message);


		bool		check_path(std::string path, Server server);

	private:
		std::map<std::string, std::string>	_header;
		std::string							_method;
		std::string							_path;
		std::string							_extension;
		std::string							_body;
		bool								_file;
};

#endif