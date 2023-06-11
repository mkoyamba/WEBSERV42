/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkoyamba <mkoyamba@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 17:24:45 by bade-lee          #+#    #+#             */
/*   Updated: 2023/06/11 14:30:04 by mkoyamba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "webserv.hpp"

class Request {
	public:
		Request(std::string Request_message, Server server);
		~Request() {};

		std::string	getMethod(void) { return _method; };
		std::string	getPath(void) { return _path; };
		std::string	getExtension(void) { return _extension; };
		bool		getFile(void) { return _file; };

		void		handle_method(std::string request_message);
		void		handle_path(std::string request_message, Server server);

		bool		check_path(std::string path, Server server);

	private:
		std::string	_method;
		std::string	_path;
		std::string	_extension;
		bool		_file;
};

#endif