/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_parser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalleze <agalleze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 14:07:33 by willysex          #+#    #+#             */
/*   Updated: 2026/06/17 13:44:11 by agalleze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "config_lexer.hpp"
#include "config.hpp"
#include <cstring>
#include <stdlib.h>

class ServerConfig;

class ConfigParser {

	public:
		ConfigParser(std::vector<Token> tokens);
		ConfigParser(const ConfigParser& other);
		ConfigParser();
		~ConfigParser();

		const ConfigParser& 					operator=(const ConfigParser& other);
		
		std::vector<Config>						Parse();

		Config		 							ParseServer();
		std::pair<std::string, LocationConfig>	ParseLocation();

		ListenInfo								ParseListen();
		std::string								ParseRoot();
		int										ParseMaxBodySize();

		void						next();
		Token						current();
		void						present(std::string expected);

		// std::vector<ListenInfo>		get_listens_infos();

	private:
		std::vector<Token>	tokens_;
		size_t 				current_;
};

#endif
