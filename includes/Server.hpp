/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkoyamba <mkoyamba@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 17:42:18 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/06/12 16:25:46 by mkoyamba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "webserv.hpp"

class Location;

class Server {
	public:
		Server() {};
		Server(std::string server);
		~Server() {};

		std::string									getName(void) { return _name; };
		std::string									getRoot(void) { return _root; };
		std::string									getIndex(void) { return _index; };
		std::string									getRedirect(void) { return _redirect; };
		std::string									getUploadPath(void) { return _upload_path; };
		std::string									getCgiPath(void) { return _cgi_path; };
		std::string									getBodySize(void) { return _body_size; };
		bool										getAutoIndex(void) { return _autoindex; };
		bool										getUpload(void) { return _upload; };
		std::string									getErrorPage(int index) { return _error_page[index]; };
		std::map<int, std::string>					getErrorPages(void) { return _error_page; };
		bool										getMethod(int index) { return _methods[index]; };
		std::vector<std::pair<std::string, int> >	getListen(void) { return _listen; }
		std::map<std::string, Location>				getLocations(void) { return _locations; };
		std::vector<std::string>					getExtensions(void) { return _cgi_ext; };

		void										handle_locations(std::string &server);
		void										handle_strings(std::string &server);
		void										handle_boolean(std::string &server);
		void										handle_errors(std::string server);
		void										handle_methods(std::string server);
		void										handle_listen(std::string server);
		void										handle_extensions(std::string server);

		int											check_content(void);



	private:
		std::map<std::string, Location>				_locations;
		std::map<int, std::string>					_error_page;
		std::vector<std::pair<std::string, int> >	_listen;
		bool										_methods[3];
		std::string									_name;
		std::string									_root;
		std::string									_index;
		std::string									_redirect;
		std::string									_upload_path;
		std::string									_cgi_path;
		std::string									_body_size;
		bool										_autoindex;
		bool										_upload;
		std::vector<std::string>					_cgi_ext;
};

#endif