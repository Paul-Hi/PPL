#include "command_line_parser.hpp"
#include "lexer.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

#ifdef WIN32

#ifndef UNICODE
#define UNICODE
#endif

#define NOMINMAX
#include <windows.h>

#ifdef WINMAIN

int main(int argc, char** argv);
int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
    AllocConsole();
    FILE* stream;
    freopen_s(&stream, "CONIN$", "r+", stdin);
    freopen_s(&stream, "CONOUT$", "w+", stderr);
    freopen_s(&stream, "CONOUT$", "w+", stdout);
    return main(__argc, __argv);
}

#endif // WINMAIN
#endif // WIN32

int main(int argc, char** argv)
{
    std::cout << "---------------------------" << std::endl;
    std::cout << "-------- PCompiler --------" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    command_line_parser cmd_parser(argc, argv);

    if (cmd_parser.cmd_option_exists("-h"))
    {
        std::cout << "---------------------------" << std::endl;
        std::cout << "------ Compiler Help ------" << std::endl;
        std::cout << "---------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << " -i  \"input file name\"   " << std::endl;
        std::cout << " -o  \"output file name\"  " << std::endl;
        std::cout << " -pp (bool) pretty print " << std::endl;
        std::cin.get();
        return 0;
    }
    if (cmd_parser.cmd_option_exists("-pp"))
    {
        // Enable Pretty Print
    }

    std::string input_file_name  = cmd_parser.get_cmd_option("-i");
    std::string output_file_name = cmd_parser.get_cmd_option("-o");

    if (input_file_name.empty())
    {
        std::cerr << "Input file is missing" << std::endl;
        std::cin.get();
        return 0;
    }

    if (output_file_name.empty())
    {
        std::string str_name = std::string(input_file_name);
        str_name             = str_name.substr(0, str_name.find_last_of("."));
        str_name             = (str_name + ".exe");
        output_file_name     = str_name.c_str();
    }

    std::cout << "  INPUT: " << input_file_name << "\n  OUTPUT: " << output_file_name << std::endl;

    std::ifstream file(input_file_name);
    std::stringstream buffer;
    buffer << file.rdbuf();

    lexer source_lexer;

    std::vector<token> tokens = source_lexer.parse(buffer.str());

    int32_t current_line         = 1;
    int32_t indent               = 0;
    static bool print_token_type = true;
    for (token t : tokens)
    {
        if (t.type == token_type::r_brace)
            indent -= 4;
        if (t.line > current_line)
        {
            std::cout << std::endl;
            current_line = t.line;
            for (int32_t i = 0; i < indent; ++i)
                std::cout << " ";
        }
        if (t.type == token_type::comment)
            std::cout << "//";
        if (print_token_type)
        {
            std::cout << token_type_to_string(t.type);
            if (t.type == token_type::identifier || t.type == token_type::string_literal || t.type == token_type::integer_literal || t.type == token_type::floating_point_literal ||
                t.type == token_type::comment)
                std::cout << " \"" << t.text << "\" ";
            else
                std::cout << " ";
        }
        else
        {
            if (t.type == token_type::string_literal)
                std::cout << "\"";
            std::cout << t.text;
            if (t.type == token_type::string_literal)
                std::cout << "\"";
            std::cout << " ";
        }
        if (t.type == token_type::l_brace)
            indent += 4;
    }

    std::cin.get();

    return 1;
}
