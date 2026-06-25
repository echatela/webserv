#pragma once

#include <string>
#include "../http_protocol/http_response.hpp"

class HttpResponse;

class Contact
{
	public:
		Contact(std::string name, std::string phone, std::string img);
		std::string	get_name() const;
		std::string	get_phone() const;
		std::string	get_img() const;
		~Contact();

		std::string HandlePostResponse(HttpResponse &response);

	private:
		std::string	name_;
		std::string	phone_;
		std::string	img_;

		Contact();
		Contact(const Contact &);
		Contact &operator=(const Contact &);
};