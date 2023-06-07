/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkoyamba <mkoyamba@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 17:32:11 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/06/07 19:37:05 by mkoyamba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "webserv.hpp"

class Location {
	public:
		Location() {};
		Location(std::string location);
		~Location() {};

		std::string		getRoot(void) { return _root; };
		std::string		getIndex(void) { return _index; };
		std::string		getRedirect(void) { return _redirect; };
		std::string		getUploadPath(void) { return _upload_path; };
		std::string		getCgiPath(void) { return _cgi_path; };
		std::string		getBodySize(void) { return _body_size; };
		bool			getAutoIndex(void) { return _autoindex; };
		bool			getUpload(void) { return _upload; };
		std::string		getErrorPage(int index) { return _error_page[index]; };
		bool			getMethod(int index) { return _methods[index]; };
	
		void	handle_strings(std::string &server);
		void	handle_boolean(std::string &server);
		void	handle_errors(std::string server);
		void	handle_methods(std::string server);

	private:
		std::map<int, std::string>	_error_page;
		bool						_methods[4];
		std::string					_body_size;
		std::string					_root;
		std::string					_index;
		std::string					_redirect;
		bool						_autoindex;
		bool						_upload;
		std::string					_upload_path;
		std::vector<std::string>	_cgi_ext;
		std::string					_cgi_path;
};

#endif