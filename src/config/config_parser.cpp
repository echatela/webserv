/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_parser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: willysex <willysex@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 15:12:45 by willysex          #+#    #+#             */
/*   Updated: 2026/07/10 11:19:07 by willysex         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config_parser.hpp"
#include "../webserv.hpp"
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <vector>
#include <netdb.h>

ConfigParser::ConfigParser(std::vector<Token> tokens)
	: tokens_(tokens), current_(0) {
}

ConfigParser::ConfigParser(const ConfigParser& other)
	: tokens_(other.tokens_), current_(other.current_) {
}

ConfigParser::ConfigParser()
	: tokens_(), current_(0) {
}

ConfigParser::~ConfigParser() {
}

const ConfigParser&	ConfigParser::operator=(const ConfigParser& other) {

	if (this != &other) {
		tokens_ = other.tokens_;
		current_ = other.current_;
	}
	return (*this);
}

void	ConfigParser::next() {
	if (current_ < tokens_.size())
		current_++;
}

Token	ConfigParser::current() {
	return (tokens_[current_]);
}

void	ConfigParser::present(std::string expected) {

	if (tokens_[current_].content != expected)
		throw std::logic_error("Expected a " + expected);
	else
		current_++;
}

static int	parsePort(std::string value) {

	char*	end;
	errno = 0;
	long	val = std::strtol(value.c_str(), &end, 10);

	if (errno == ERANGE || end == value.c_str() || *end != '\0'
		|| val < 1 || val > 65535)
		throw std::logic_error("Port not valid: " + value);
	return (static_cast<int>(val));
}

ListenInfo	ConfigParser::ParseListen() {

	present("listen");

	ListenInfo	listen_info;
	std::string	token = current().content;
	std::string	host = "0.0.0.0";
	std::string	port = token;
	size_t		sep = token.find(':');

	if (sep != std::string::npos) {
		host = token.substr(0, sep);
		port = token.substr(sep + 1);
	}
	parsePort(port);

	struct addrinfo		hints;
	struct addrinfo*	res = NULL;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int	rc = getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
	if (rc != 0 || res == NULL)
		throw std::logic_error("listen" + token + ": "
			+ gai_strerror(rc));

	listen_info.host = host;
	listen_info.port = port;
	memcpy(&listen_info.address, res->ai_addr, sizeof(listen_info.address));
	freeaddrinfo(res);

	current_++;
	present(";");
	return listen_info;
}

std::string 		ConfigParser::ParseRoot() {
	
	present("root");
	std::string root = current().content;
	// check valid
	current_++;
	present(";");
	return (root);
}

std::string			ConfigParser::ParseCgi() {
	
	present("cgi");
	std::string cgi = current().content;

	current_++;
	present(";");
	return (cgi);
}

std::vector<std::string>			ConfigParser::ParseStr(std::string directive) {

	present(directive);
	std::vector<std::string> result;
	while (current().content != ";")
	{
		if (directive == "root" && current().content[0] == '.')
			result.push_back(current().content.substr(1));
		else
			result.push_back(current().content);
		current_++;
	}
	present(";");
	return result;
}

size_t	ConfigParser::ParseMaxBodySize() {
	
	present("client_max_body_size");
	size_t max_size = webserv::utils::ParseSize(current().content);

	current_++;
	present(";");
	return (max_size);
}

bool	ConfigParser::ParseUploadBool() {

	present("upload");

	bool	upload;
	if (current().content == "on")
		upload = true;
	else if (current().content == "off")
		upload = false;
	else
		throw std::logic_error("Invalid upload argument");
	current_++;
	present(";");
	return upload;
}

std::pair<std::string, LocationConfig>	ConfigParser::ParseLocation() {

	std::string	base;
	LocationConfig	directives;

	present("location");
	base = current().content;
	directives.base_location = base;
	current_++;
	present("{");
	directives.autoindex = false;
	directives.upload_enabled = false;
	
	while (current_ < tokens_.size() && current().content != "}")
	{
		if (current().content == "root")
			directives.root = ParseStr("root");
		else if (current().content == "cgi")
			directives.cgi = ParseStr("cgi");
		else if (current().content == "methods")
			directives.methods = ParseStr("methods");
		else if (current().content == "index")
			directives.index = ParseStr("index");
		else if (current().content == "autoindex") {
			std::vector<std::string> tmp = ParseStr("autoindex");
			if (tmp.size() != 1
				|| (tmp[0] != "on" && tmp[0] != "off"))
				throw std::runtime_error(
					"Invalid argument: autoindex");
			directives.autoindex = (tmp[0] == "on");
		} else if (current().content == "return") {
			directives.redirect = ParseStr("return");
			if (directives.redirect.size() != 2)
				throw std::runtime_error(
					"Invalid argument: return");
		}
		else if (current().content == "upload")
			directives.upload_enabled = ParseUploadBool();
		else if (current().content == "upload_path") {
			std::vector<std::string> tmp = ParseStr("upload_path");
			if (tmp.size() != 1)
				throw std::runtime_error(
					"Invalid argument: upload_path");
			directives.upload_path = tmp[0];
		}
		else
			throw std::logic_error("Unknown directive: "
				+ current().content);
	}
	present("}");
	std::pair<std::string, LocationConfig> location(base, directives);

	return (location);
}

std::pair<int, std::string> ConfigParser::ParseErrorPage() {

	present("error_page");
	
	int code = webserv::utils::ParseUInt(current().content);
	if (code < 100 || code > 599)
		throw std::runtime_error("Invalid error code.");
	current_++;
	
	std::string url = current().content;
	current_++;
	
	present(";");
	
	std::pair<int, std::string>	page(code, url);

	return page;
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
		else if (current().content == "client_max_body_size")
			server.set_max_body_size(ParseMaxBodySize());
		else if (current().content == "error_page")
			server.add_error_page(ParseErrorPage());
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
