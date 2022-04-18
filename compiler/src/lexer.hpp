#ifndef LEXER_HPP
#define LEXER_HPP

#include "token.hpp"
#include <vector>

class lexer
{
  public:
    lexer()  = default;
    ~lexer() = default;

    std::vector<token> parse(const std::string& source);

  private:
    void begin_token(token& current, int32_t line, int32_t inline_offset);
    void end_token(token& current, std::vector<token>& token_list);
};

#endif LEXER_HPP
