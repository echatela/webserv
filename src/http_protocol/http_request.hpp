#pragma once

#include <string>
#include <map>

class httpRequest
{
	public :
		httpRequest();	
		~httpRequest();
		
		void setMethod(const std::string& method);
		void setPath(const std::string& path);
		void setVersion(const std::string& version);
		void setHeader(const std::map<std::string, std::string>& header);
		void setBody(const std::string& body);
		
		const std::string getMethod() const;
		const std::string getPath() const;
		const std::string getVersion() const;
		const std::map<std::string, std::string> getHeader() const;
		const std::string getBody() const;
		
	private :
		httpRequest(const httpRequest& other);
		httpRequest& operator=(const httpRequest& other);

		std::string method_;
		std::string path_;
		std::string version_;
		std::map<std::string, std::string> header_;
		std::string body_;
};