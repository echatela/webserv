#pragma once

#include <string>
#include <iostream>

class User
{
public:
    User(std::string name);
    ~User();

    std::string	getName() const;

    private:
    std::string	_name;
};