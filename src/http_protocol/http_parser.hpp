#pragma once

#include <string>
#include <iostream>

class httpParser
{
	public:
		// httpParser(const std::string buf, size_t n);
		~httpParser();

		int add(const std::string buf, size_t n);

		const int getFlag() const;
		const std::string& getBuf() const;
		
	private :
		httpParser();
		httpParser(const httpParser& other);
		httpParser& operator=(const httpParser& other);

		std::string buf_;
		int flag_;
};