#include "User.hpp"

User::User(std::string name) : _name(name)
{ }

User::~User() { }

std::string	User::getName() const
{
    return _name;
}