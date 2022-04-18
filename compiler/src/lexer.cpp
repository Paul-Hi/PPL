#include "lexer.hpp"
#include <iostream>

std::vector<token> lexer::parse(const std::string& source)
{
    std::vector<token> token_list;

    token current;
    int32_t line          = 1;
    int32_t inline_offset = 1;

    auto iter = source.cbegin();
    while (iter != source.cend())
    {
        inline_offset++;

        if (current.type == token_type::comment && !(*iter == '\n' || *iter == '\r'))
        {
            current.text.append(1, *iter);
            ++iter;
            continue;
        }
        if (current.type == token_type::string_literal && *iter != '"')
        {
            current.text.append(1, *iter);
            ++iter;
            continue;
        }

        switch (*iter)
        {
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case '_':
            if (current.type != token_type::identifier)
            {
                if (current.type == token_type::string_literal || current.type == token_type::integer_literal || current.type == token_type::floating_point_literal ||
                    current.type == token_type::boolean_literal)
                {
                    // continue and print more errors, if there are any
                    std::cerr << "Unkown token \'" << *iter << "\' in " << current.text << " at (" << line << ", " << inline_offset << ")" << std::endl;
                }
                end_token(current, token_list);
                begin_token(current, line, inline_offset);
                current.type = token_type::identifier;
                current.text.append(1, *iter);
            }
            else
            {
                current.text.append(1, *iter);
            }
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            if (!(current.type == token_type::integer_literal || current.type == token_type::floating_point_literal))
            {
                if (current.type == token_type::identifier)
                {
                    current.text.append(1, *iter);
                }
                else
                {
                    end_token(current, token_list);
                    begin_token(current, line, inline_offset);
                    current.type = token_type::integer_literal;
                    current.text.append(1, *iter);
                }
            }
            else
            {
                current.text.append(1, *iter);
            }
            break;
        case '"':
            if (current.type != token_type::string_literal)
            {
                end_token(current, token_list);
                begin_token(current, line, inline_offset);
                current.type = token_type::string_literal;
            }
            else
            {
                end_token(current, token_list);
            }
            break;
        case '+':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::plus;
            current.text.append(1, *iter);
            if (*(iter + 1) == '+') // ++ check
            {
                current.type = token_type::increment;
                current.text.append(1, *(++iter));
            }
            end_token(current, token_list);
            break;
        case '-':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::minus;
            current.text.append(1, *iter);
            if (*(iter + 1) == '-') // -- check
            {
                current.type = token_type::decrement;
                current.text.append(1, *(++iter));
            }
            else if (*(iter + 1) == '>') // -> check
            {
                current.type = token_type::transmutation_arrow;
                current.text.append(1, *(++iter));
            }
            end_token(current, token_list);
            break;
        case '*':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::star;
            current.text.append(1, *iter);
            end_token(current, token_list);
        case '/':
            end_token(current, token_list);
            if (*(iter + 1) == '/') // comment check
            {
                current.type = token_type::comment;
                iter += 2;
                continue;
            }
            begin_token(current, line, inline_offset);
            current.type = token_type::slash;
            current.text.append(1, *iter);
            end_token(current, token_list);
            break;
        case '%':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::percent;
            current.text.append(1, *iter);
            end_token(current, token_list);
            break;
        case '=':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::assign;
            current.text.append(1, *iter);
            if (*(iter + 1) == '=') // == check
            {
                current.type = token_type::equal;
                current.text.append(1, *(++iter));
            }
            end_token(current, token_list);
            break;
        case '<':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::less_then;
            current.text.append(1, *iter);
            if (*(iter + 1) == '=') // <= check
            {
                current.type = token_type::less_then_or_equal;
                current.text.append(1, *(++iter));
            }
            else if (*(iter + 1) == '<') // << check
            {
                current.type = token_type::shift_left;
                current.text.append(1, *(++iter));
            }
            end_token(current, token_list);
            break;
        case '>':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::greather_then;
            current.text.append(1, *iter);
            if (*(iter + 1) == '=') // >= check
            {
                current.type = token_type::greather_then_or_equal;
                current.text.append(1, *(++iter));
            }
            else if (*(iter + 1) == '>') // >> check
            {
                current.type = token_type::shift_right;
                current.text.append(1, *(++iter));
            }
            end_token(current, token_list);
            break;
        case '!':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::exclamation_mark;
            current.text.append(1, *iter);
            if (*(iter + 1) == '=') // != check
            {
                current.type = token_type::not_equal;
                current.text.append(1, *(++iter));
            }
            end_token(current, token_list);
            break;
        case '&':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::ampersand;
            current.text.append(1, *iter);
            if (*(iter + 1) == '&') // && check
            {
                current.type = token_type::logical_and;
                current.text.append(1, *(++iter));
            }
            end_token(current, token_list);
            break;
        case '|':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::pipe;
            current.text.append(1, *iter);
            if (*(iter + 1) == '&') // && check
            {
                current.type = token_type::logical_or;
                current.text.append(1, *(++iter));
            }
            end_token(current, token_list);
            break;
        case '^':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::circumflex;
            current.text.append(1, *iter);
            end_token(current, token_list);
            break;
        case '(':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::l_parentheses;
            current.text.append(1, *iter);
            if (*(iter + 1) == ')') // () check
            {
                current.type = token_type::unit;
                current.text.append(1, *(++iter));
            }
            end_token(current, token_list);
            break;
        case ')':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::r_parentheses;
            current.text.append(1, *iter);
            end_token(current, token_list);
            break;
        case '{':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::l_brace;
            current.text.append(1, *iter);
            end_token(current, token_list);
            break;
        case '}':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::r_brace;
            current.text.append(1, *iter);
            end_token(current, token_list);
            break;
        case '[':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::l_bracket;
            current.text.append(1, *iter);
            end_token(current, token_list);
            break;
        case ']':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::r_bracket;
            current.text.append(1, *iter);
            end_token(current, token_list);
            break;
        case ',':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::comma;
            current.text.append(1, *iter);
            end_token(current, token_list);
            break;
        case ';':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::semicolon;
            current.text.append(1, *iter);
            end_token(current, token_list);
            break;
        case '.':
            if (current.type == token_type::integer_literal)
            {
                current.type = token_type::floating_point_literal;
                current.text.append(1, *iter);
            }
            else
            {
                // member access operator
                end_token(current, token_list);
                begin_token(current, line, inline_offset);
                current.type = token_type::point;
                current.text.append(1, *iter);
                end_token(current, token_list);
            }
            break;
        case ':':
            end_token(current, token_list);
            begin_token(current, line, inline_offset);
            current.type = token_type::colon;
            if (*(iter + 1) == ':') // :: check
            {
                current.type = token_type::double_colon;
                current.text.append(1, *(++iter));
            }
            current.text.append(1, *iter);
            end_token(current, token_list);
            break;
        case ' ':
        case '\t':
            end_token(current, token_list);
            break;
        case '\n':
            end_token(current, token_list);
            line++;
            inline_offset = 0;
            break;
        case '\r':
            end_token(current, token_list);
            line++;
            inline_offset = 0;
            if (*(iter + 1) == '\n') // \r\n check
                ++iter;
            break;
        default:
            break;
        }
        ++iter;
    }

    std::cout << std::endl;
    std::cout << "Source Info: " << line + 1 << " lines, " << source.length() << " characters." << std::endl;

    return token_list;
}

void lexer::begin_token(token& current, int32_t line, int32_t inline_offset)
{
    current.line          = line;
    current.inline_offset = inline_offset;
}

void lexer::end_token(token& current, std::vector<token>& token_list)
{
    if (current.type != token_type::undefined)
    {
        token_list.push_back(current);
    }

    current.type = undefined;
    current.text.erase();
    current.line          = -1;
    current.inline_offset = -1;
}
