#pragma once

#include "http_parser.hpp"
#include <sstream>
#include <stdexcept>
#include <string>
#include <map>
#include <ostream>

class httpRequest
{
	public :
		httpRequest();	
		~httpRequest();
		
		httpRequest& operator=(const httpRequest& other);
		const std::string getMethod() const;
		const std::string getPath() const;
		const std::string getVersion() const;
		const std::map<std::string, std::string> getHeader() const;
		const std::string getBody() const;

		int parseRequestLine(std::string requestline);
		int parseMethod(const std::string& method);
		int parsePath(const std::string& path);
		int parseVersion(const std::string& version);
		int parseHeader(std::string header);
		int parseBody(std::string body);
		
	private :
		httpRequest(const httpRequest& other);

		std::string method_;
		std::string path_;
		std::string version_;
		std::map<std::string, std::string> header_;
		std::string body_;
};
