//! \file      lexer.hpp
//! \author    Paul Himmler
//! \version   1.0
//! \date      2022
//! \copyright Apache License 2.0

#ifndef LEXER_HPP
#define LEXER_HPP

#include "token.hpp"
#include <vector>
#include <unordered_map>

class lexer
{
  public:
    lexer();
    ~lexer() = default;

    std::vector<token> parse(const std::string& source);

  private:
    void begin_token(token& current, int32_t line, int32_t inline_offset);
    void end_token(token& current, std::vector<token>& token_list);

    void scan_identifier(std::string::const_iterator& iter, token& current);
    void scan_number(std::string::const_iterator& iter, token& current, int32_t line, int32_t& inline_offset);

    std::unordered_map<std::string, token_type> predefined;
};

#endif LEXER_HPP
