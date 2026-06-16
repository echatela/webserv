/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_parser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalleze <agalleze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 15:12:45 by willysex          #+#    #+#             */
/*   Updated: 2026/06/16 13:57:39 by agalleze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config_parser.hpp"

ConfigParser::ConfigParser(std::vector<Token> tokens) : tokens_(tokens), current_(0) {}

ConfigParser::ConfigParser(const ConfigParser& other) : tokens_(other.tokens_), current_(other.current_) {}

ConfigParser::ConfigParser() : tokens_(), current_(0) {}

ConfigParser::~ConfigParser() {}

const ConfigParser&			ConfigParser::operator=(const ConfigParser& other) {
	if (this != &other)
	{
		tokens_ = other.tokens_;
		current_ = other.current_;
	}
	return (*this);
}

void	ConfigParser::next() {
	if (current_ < tokens_.size())
		current_++;
}

Token 						ConfigParser::current() {
	return (tokens_[current_]);
}

void						ConfigParser::present(std::string expected) {
	if (tokens_[current_].content != expected)
		throw std::logic_error("Expected a " + expected);
	else
		current_++;
}

static int		parsePort(std::string value)
{
	char*	end;
	long	val = std::strtol(value.c_str(), &end, 10);

	if (errno == ERANGE || val < 0 || val > 65535)
		throw std::logic_error("Port value not valid: " + value);
		
	return (static_cast<int>(val));
}
ListenInfo 				ConfigParser::ParseListen() {
	present("listen");
	ListenInfo	listen_info;
	int			port = parsePort(current().content);

	listen_info.port = current().content;
	memset(&listen_info.address, 0, sizeof(listen_info.address));
	listen_info.address.sin_family = AF_INET;
	listen_info.address.sin_port = htons(port);
	listen_info.address.sin_addr.s_addr = htonl(INADDR_ANY);

// check valid
	current_++;
	present(";");
	return (listen_info);
}

std::string 		ConfigParser::ParseRoot() {
	
	present("root");
	std::string root = current().content;
	// check valid
	current_++;
	present(";");
	return (root);
}

std::pair<std::string, LocationConfig> 				ConfigParser::ParseLocation() {

	std::string		base;
	LocationConfig directives;
	present("location");
	base = current().content;
	current_++;
	present("{");
	
	while (current_ < tokens_.size() && current().content != "}")
	{
		if (current().content == "root")
		directives.root = ParseRoot();
		else
		throw std::logic_error("Unknown directive: " + current().content);
	}
	present("}");
	std::pair<std::string, LocationConfig> location(base, directives);

	return (location);
}

Config		 				ConfigParser::ParseServer() {

	Config server;

	present("server");
	present("{");

	while (current_ < tokens_.size() && current().content != "}")
	{
		if (current().content == "listen")
			server.add_listen_info(ParseListen());
		else if (current().content == "root")
			server.set_root(ParseRoot());
		else if (current().content == "location")
			server.add_location(ParseLocation());
		else
			throw std::logic_error("Unknown directive: " + current().content);
	}
	present("}");

	return (server);
}


std::vector<Config> 	ConfigParser::Parse() {

	std::vector<Config> 	servers;
	
	while (current_ < tokens_.size() && current().content != "EOF")
	{
		if (current().content == "server")
		{
			servers.push_back(ParseServer());
		}
		else
			throw std::logic_error("Unknown directive: " + current().content);
	}
	return (servers);
}
