/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkoyamba <mkoyamba@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 17:09:10 by mkoyamba          #+#    #+#             */
/*   Updated: 2023/06/21 17:09:39 by mkoyamba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"

int	main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "usage: ./webserv [configuration file]" << std::endl;
		return 1;
	}
	std::string filename(argv[1]);
	Config config;
	try {config.handle_file(filename); }
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	int check = config.check_content();
	if (check)
		return check;
	print_servers(config);
	std::cout << PURPLE_B << "Listening to ports: ";
	check = split_servers(config);
	return check;
}