//! \file      command_line_parser.hpp
//! \author    Paul Himmler
//! \version   1.0
//! \date      2022
//! \copyright Apache License 2.0

#ifndef COMMAND_LINE_PARSER_HPP
#define COMMAND_LINE_PARSER_HPP

#include <iostream>
#include <vector>

class command_line_parser
{
  public:
    command_line_parser(int32_t argc, char** argv)
    {
        this->argv = std::vector<std::string>(argv, argv + argc);
    };

    ~command_line_parser() = default;

    bool cmd_option_exists(const std::string& option)
    {
        return std::find(this->argv.begin(), this->argv.end(), option) != this->argv.end();
    }

    std::string get_cmd_option(const std::string& option)
    {
        auto iter = std::find(this->argv.begin(), this->argv.end(), option);
        if (iter != this->argv.end() && ++iter != this->argv.end())
        {
            return *iter;
        }
        return std::string();
    }

  private:
    std::vector<std::string> argv;
};

#endif COMMAND_LINE_PARSER_HPP
