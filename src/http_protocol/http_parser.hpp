#pragma once

#include <string>
#include <iostream>

class httpParser
{
	public:
		httpParser();
		~httpParser();

		int add(const std::string buf, size_t n);

		int getFlag() const;
		std::string getBuf() const;
		
	private :
		httpParser(const httpParser& other);
		httpParser& operator=(const httpParser& other);

		std::string buf_;
		int flag_;
};