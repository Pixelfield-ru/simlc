#include "../src/Node/Node.h"
#include "../src/Lexer/Lexer.h"
#include "../src/Lexer/Source.h"
#include <iostream> // For std::cout, std::cerr
#include <fstream>  // For std::ifstream
#include <sstream>  // For std::stringstream
#include <string>   // For std::string
#include <iostream>
#include <string_view>

void cout_all_tokens(SIML::Lexer& lexer);

#define assert_fail(str)



int main() {
    std::string str = ".test: \"hello\"; some {.number: 3.6;}";
    auto source = SIML::Source(str);
    auto lexer = SIML::Lexer(source);


    auto node = SIML::NodeObject::parseAsGlobalNode(lexer);
    if (node.hasError()) {
        std::cout << "Error: " << node.error().m_message;
        return 0;
    }

    std::cout << **node;

    // cout_all_tokens(lexer);

    return 0;
}

void cout_all_tokens(SIML::Lexer& lexer) {
    while (auto token = lexer.peek()) {
        std::string_view test;
        switch (*token) {
            case SIML::TokenType::DOT: 
                std::cout << "." << std::endl;
                lexer.consume_next();
                break;
            case SIML::TokenType::BLOCK_CLOSE:
                std::cout << "}" << std::endl;
                lexer.consume_next();
                break;
            case SIML::TokenType::IDENT:
                test = lexer.getNextIdent();
                std::cout << test << std::endl;
                break;
            case SIML::TokenType::STRING:
                std::cout << lexer.getNextString().value() << std::endl;
                break;
            case SIML::TokenType::NUMBER:
                std::cout << lexer.getNextNumber() << std::endl;
                break;
            case SIML::TokenType::BLOCK_OPEN:
                std::cout << "{" << std::endl;
                lexer.consume_next();
                break;
            case SIML::TokenType::EXPR_END:
                std::cout << ";" << std::endl;
                lexer.consume_next();
                break;
            case SIML::TokenType::TEXT_BLOCK_OPEN:
                std::cout << "[" << std::endl;
                lexer.consume_next();
                break;
            case SIML::TokenType::TEXT_BLOCK_CLOSE:
                std::cout << "]" << std::endl;
                lexer.consume_next();
                break;
            case SIML::TokenType::DOUBLE_DOT:
                std::cout << ":" << std::endl;
                lexer.consume_next();
                break;
        }
    }
}