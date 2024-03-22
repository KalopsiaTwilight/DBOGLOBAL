#include "stdafx.h"
#include "comutils.h"
#include "stdexcept"

bool comutils::parse_double(std::string in, double& res)
{
	try {
		size_t read = 0;
		res = std::stod(in, &read);
		if (in.size() != read)
			return false;
	}
	catch (std::invalid_argument) {
		return false;
	}
	return true;
}

bool comutils::parse_float(std::string in, float& res)
{
	try {
		size_t read = 0;
		res = std::stof(in, &read);
		if (in.size() != read)
			return false;
	}
	catch (std::invalid_argument) {
		return false;
	}
	return true;
}

bool comutils::parse_int(std::string in, int& res)
{
	try {
		size_t read = 0;
		res = std::stoi(in, &read);
		if (in.size() != read)
			return false;
	}
	catch (std::invalid_argument) {
		return false;
	}
	return true;
}

bool comutils::parse_byte(std::string in, byte& res)
{
	int intRes;
	if (parse_int(in, intRes)) {
		if (intRes < CHAR_MAX) {
			res = (BYTE)intRes;
			return true;
		}
	}
	return false;
}

bool comutils::parse_long(std::string in, long& res)
{
	try {
		size_t read = 0;
		res = std::stol(in, &read);
		if (in.size() != read)
			return false;
	}
	catch (std::invalid_argument) {
		return false;
	}
	return true;
}
bool comutils::parse_llong(std::string in, long long& res)
{
	try {
		size_t read = 0;
		res = std::stoll(in, &read);
		if (in.size() != read)
			return false;
	}
	catch (std::invalid_argument) {
		return false;
	}
	return true;
}

bool comutils::parse_uint(std::string in, unsigned int& res)
{
	unsigned long lRes;
	if (parse_ulong(in, lRes)) {
		if (lRes < UINT_MAX) {
			res = (unsigned int)lRes;
			return true;
		}
	}
	return false;
}

bool comutils::parse_ulong(std::string in, unsigned long& res)
{
	try {
		size_t read = 0;
		res = std::stoul(in, &read);
		if (in.size() != read)
			return false;
	}
	catch (std::invalid_argument) {
		return false;
	}
	return true;
}