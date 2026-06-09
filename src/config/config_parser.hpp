/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_parser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalleze <agalleze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 14:07:33 by willysex          #+#    #+#             */
/*   Updated: 2026/06/09 15:59:07 by agalleze         ###   ########.fr       */
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

		const ConfigParser& 		operator=(const ConfigParser& other);
		ServerConfig 				ParseServer();
		LocationConfig 				ParseLocation();
		ListenInfo					ParseListen();
		std::string					ParseRoot();
		std::vector<ServerConfig>	Parse();
		void						next();
		Token						current();
		void						present(std::string expected);

	private:
		std::vector<Token>	tokens_;
		size_t 				current_;
};

#endif
