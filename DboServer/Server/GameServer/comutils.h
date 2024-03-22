#pragma once
namespace comutils
{
    bool parse_double(std::string in, double& res);

    bool parse_float(std::string in, float& res);

    bool parse_int(std::string in, int& res);

    bool parse_byte(std::string in, byte& res);

    bool parse_long(std::string in, long& res);

    bool parse_llong(std::string in, long long& res);

    bool parse_uint(std::string in, unsigned int& res);

    bool parse_ulong(std::string in, unsigned long& res);
};

