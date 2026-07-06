// #pragma once
#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include "http_parser.hpp"
#include <string>
#include <map>

class HttpRequest
{
	public :
		HttpRequest();	
		~HttpRequest();
		
		HttpRequest&		operator=(const HttpRequest& other);

		const std::string &	method() const;
		const std::string &	path() const;
		const std::string &	version() const;
		std::string		header(std::string key) const; 
		const std::map<std::string, std::string> &	headers() const;
		const std::string &	body() const;
		int			error() const;
		
		void set_error(int error);

		int ParseRequestLine(std::string requestline);
		int ParseMethod(const std::string& method);
		int ParsePath(const std::string& path);
		int ParseVersion(const std::string& version);
		int ParseHeader(std::string header);
		int ParseBody(std::string body);

	private :
		HttpRequest(const HttpRequest& other);

		std::string method_;
		std::string path_;
		std::string version_;
		std::map<std::string, std::string> header_;
		std::string body_;

		int error_;
};

#endif
