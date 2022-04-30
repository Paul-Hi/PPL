#include "ast.hpp"
#include "ast_visitor.hpp"
#include "command_line_parser.hpp"
#include "lexer.hpp"
#include "parser.hpp"
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
    std::cout << "----------------------------" << std::endl;
    std::cout << "------- PPL Compiler -------" << std::endl;
    std::cout << "----------------------------" << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    command_line_parser cmd_parser(argc, argv);

    if (cmd_parser.cmd_option_exists("-h"))
    {
        std::cout << "----------------------------" << std::endl;
        std::cout << "----------- Help -----------" << std::endl;
        std::cout << "----------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << "  -i  \"input file name\"     " << std::endl;
        std::cout << "  -o  \"output file name\"    " << std::endl;
        std::cout << "  -pp (bool) pretty print   " << std::endl;
        std::cout << "  -dot (bool) plot ast  " << std::endl;
        std::cin.get();
        return 0;
    }
    bool dot_ast = cmd_parser.cmd_option_exists("-dot");
    bool pretty_print = cmd_parser.cmd_option_exists("-pp");

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
    std::cout << std::endl;
    std::cout << std::endl;

    std::ifstream file(input_file_name);
    std::stringstream buffer;
    buffer << file.rdbuf();

    lexer source_lexer;

    std::vector<token> tokens = source_lexer.parse(buffer.str());

    std::cout << std::endl;
    std::cout << std::endl;

    parser token_parser(tokens);

    unique_ptr<expression> program_node = token_parser.parse();

    std::cout << std::endl;
    std::cout << std::endl;

    if (dot_ast)
    {
        int32_t out = 0;
        dot_visitor v("graph.dot");
        v(*program_node, out);
    }

    std::cout << "Done" << std::endl;
    std::cin.get();

    return 0;
}
