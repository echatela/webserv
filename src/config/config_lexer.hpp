/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_lexer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalleze <agalleze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 12:31:16 by agalleze          #+#    #+#             */
/*   Updated: 2026/06/09 15:57:38 by agalleze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_LEXER_HPP
#define CONFIG_LEXER_HPP

#include <vector>
#include <string>


enum TokenType {
	WORD,
	OPEN_BRACKET,
	CLOSE_BRACKET,
	SEMICOLON,

};

struct Token {

	TokenType	type;
	std::string	content;

};

class ConfigLexer {
	
	public:
		ConfigLexer(std::string filename);
		ConfigLexer(const ConfigLexer& other);
		~ConfigLexer();

		const ConfigLexer&	operator=(const ConfigLexer& other);

		std::vector<Token>	Tokenize();
		static Token		MakeToken(std::string current);
		static TokenType	type(std::string current);

	private:
		std::string			file_content_;

};

#endif
