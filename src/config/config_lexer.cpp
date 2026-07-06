/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_lexer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalleze <agalleze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 14:19:42 by agalleze          #+#    #+#             */
/*   Updated: 2026/06/17 14:08:54 by agalleze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config_lexer.hpp"
#include <fstream>
#include <stdexcept>

ConfigLexer::ConfigLexer(std::string filename) {

	if (filename.compare(filename.size() - 5, 5, ".conf"))
		throw std::logic_error("Missing file extension <.conf>");

	// std::cout << "filename ." << filename.c_str() << std::endl;
	std::ifstream 	file(filename.c_str());
	std::string	line;
	std::string	eof = "EOF";


	if (!file.is_open())
		throw std::runtime_error("Could not open config file");

	while (std::getline(file, line))
	{
		if (line[0] != '#')
			file_content_.append(line);
	}

	file_content_.append(eof);
	file.close();
}
	
ConfigLexer::ConfigLexer(const ConfigLexer& other) : file_content_(other.file_content_) {}

ConfigLexer::~ConfigLexer() {}

const ConfigLexer&	ConfigLexer::operator=(const ConfigLexer& other) {
	if (this != &other)
		this->file_content_ = other.file_content_;
	return *this;
}

TokenType			ConfigLexer::type(std::string current) {
	std::string		characters = "{;}";
		size_t type = characters.find(current[0]);
		switch (type)
		{
			case 0:
				return (OPEN_BRACKET);
			case 1:
				return (SEMICOLON); 
			case 2:
				return (CLOSE_BRACKET); 
			default:
				return (WORD);
		}
	
	return (WORD);
}

Token				ConfigLexer::MakeToken(std::string str) {
	Token current;

	current.content = str;
	current.type = type(str);
	return (current);
}

bool 	isSpecial(char c) {
	return (c == ';' || c == '{' || c == '}');
}

std::vector<Token>	ConfigLexer::Tokenize() {

	std::vector<Token> tokens;
	std::string		current_str;
	size_t i = 0;
	while (i < file_content_.size())
	{
		while (i < file_content_.size() 
			&& !std::isspace(file_content_[i])
			&& !isSpecial(file_content_[i]))
			current_str += file_content_[i++];
		if (!current_str.empty())
		{
			tokens.push_back(MakeToken(current_str));
			current_str.clear();
		}
		if (isSpecial(file_content_[i]))
		{
			current_str += file_content_[i];
			if (!current_str.empty())
			{
				tokens.push_back(MakeToken(current_str));
				current_str.clear();
			}
		}
		i++;
	}
	
	// for (size_t i = 0; i < tokens.size(); i++)
	// {
	// 	std::cout << "token type: " << tokens[i]._type << "token content: " << tokens[i]._content << std::endl;
	// }
	return (tokens);
}

