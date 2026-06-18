#pragma once

#include <string>
#include "http_response.hpp"

class HttpResponse;

class Contact
{
	public:
		Contact(std::string name, std::string phone, void *img);
		std::string	get_name() const;
		std::string	get_phone() const;
		void *		get_img() const;
		~Contact();

		static std::string HandlePostResponse(HttpResponse &response);

	private:
		std::string	name_;
		std::string	phone_;
		void *img_;

		Contact();
		Contact(const Contact &);
		Contact &operator=(const Contact &);
};