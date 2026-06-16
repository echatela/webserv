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
		const std::string get_method() const;
		const std::string get_path() const;
		const std::string get_version() const;
		const std::map<std::string, std::string> get_header() const;
		const std::string get_body() const;
		int get_error() const;
		
		void set_error(int error);

		int ParseRequestLine(std::string requestline);
		int ParseMethod(const std::string& method);
		int ParsePath(const std::string& path);
		int ParseVersion(const std::string& version);
		int ParseHeader(std::string header);
		int ParseBody(std::string body);

	private :
		httpRequest(const httpRequest& other);

		std::string method_;
		std::string path_;
		std::string version_;
		std::map<std::string, std::string> header_;
		std::string body_;

		int error_;
};
