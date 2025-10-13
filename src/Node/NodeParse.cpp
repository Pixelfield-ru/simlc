#include "./Node.h"
#include "../Lexer/Lexer.h"
#include <memory>
#include <utility>
#include <optional>
#include <variant>

Expected<std::unique_ptr<SIML::NodeObject>, SIML::ParseError> parse_next_object(SIML::Lexer& lexer) noexcept;

Expected<std::unique_ptr<SIML::Node>, SIML::ParseError> parse_next_node(SIML::Lexer& lexer) noexcept {
    auto token = lexer.peek();
    if (!token) {return Unexpected(SIML::ParseError("Expected value", lexer));}
    
    if (*token == SIML::TokenType::BLOCK_CLOSE | 
        *token == SIML::TokenType::TEXT_BLOCK_CLOSE |
        *token == SIML::TokenType::EXPR_END
    ) {
        return Unexpected(
            SIML::ParseError("Unexpected token " + SIML::Lexer::tokenTypeToPreview(*token), lexer)
        );
    } else if (*token == SIML::TokenType::BLOCK_OPEN) {
        auto temp = parse_next_object(lexer); reterr(temp);
        return std::move(*temp);
    } else if (*token == SIML::TokenType::IDENT) { // ident <value> OR ident;
        auto ident = lexer.getNextIdent();
        auto next = lexer.peek();
        if (next && *next != SIML::TokenType::EXPR_END) {
            auto component = std::make_unique<SIML::NodeComponent>();
            component->m_name = ident;
            expectset(auto value, parse_next_node(lexer));
            component->m_value = std::move(value);
            return std::move(component);
        } else { // next exists AND it ;
            lexer.consume_next(); // consume ;

            auto ident_node = std::make_unique<SIML::NodeIdent>();
            ident_node->ident = ident;
            return std::move(ident_node);
        }
    } else if (*token == SIML::TokenType::STRING) { // "..."
        auto node = std::make_unique<SIML::NodeString>();
        auto temp = lexer.getNextString(); reterr(temp);
        node->m_unescapedValue = std::move(*temp);
        
        auto token = lexer.peek();
        if (token && *token == SIML::TokenType::IDENT) {
            node->tag = lexer.getNextIdent();
        }

        // REQUIRED ;
        token = lexer.peek(); lexer.consume_next();
        if (token != SIML::TokenType::EXPR_END) {
            return Unexpected(SIML::ParseError("';' expected", lexer));
        }
        
        return std::move(node);
    } else if (*token == SIML::TokenType::NUMBER | *token == SIML::TokenType::DOT) { // X.X | .X | X.
        auto node = std::make_unique<SIML::NodeNumber>();
        auto token = lexer.peek();

        if (token == SIML::TokenType::NUMBER) {
            node->m_rawIntegerPart = lexer.getNextNumber();
        }

        token = lexer.peek();
        if (token && *token == SIML::TokenType::DOT) {
            node->m_rawFloatPart = std::make_optional<std::optional<std::string_view>>(std::nullopt);
            
            lexer.consume_next();
            token = lexer.peek();

            if (token == SIML::TokenType::NUMBER) {
                node->m_rawFloatPart = lexer.getNextNumber();
            }
        }

        token = lexer.peek();
        if (token && *token == SIML::TokenType::IDENT) {
            node->m_tag = lexer.getNextIdent();
        }

        // REQUIRED ;
        token = lexer.peek(); lexer.consume_next();
        if (token != SIML::TokenType::EXPR_END) {
            return Unexpected(SIML::ParseError("\";\" expected", lexer));
        }

        return std::move(node);
    }
}

Expected<Unit, SIML::ParseError> parse_object_element(SIML::Lexer& lexer, SIML::NodeObject& obj, SIML::TokenType token) noexcept {
    if (token == SIML::TokenType::DOT) { // Parse property
        // EDGE CASE; .<ident> vs .<number>
        // If it is .<number> return back and parse as value
        // (THIS IS UGLY)
        auto cursor = lexer.m_source.m_pointer;
        
        lexer.consume_next();

        if (lexer.peek() == SIML::TokenType::NUMBER) {
            lexer.m_source.m_pointer = cursor;

            auto temp = parse_next_node(lexer); reterr(temp);
            auto val = std::move(*temp);
            obj.m_positionalProperties.push_back(std::move(val));
            return Unit {};
        }

        // EDGE CASE END

        if (lexer.peek() != SIML::TokenType::IDENT) {
            // TODO: error: unexcpected token
        }

        auto ident = lexer.getNextIdent();

        // Optional ":"
        if (lexer.peek() == SIML::TokenType::DOUBLE_DOT) {lexer.consume_next();}

        expectset(auto value, parse_next_node(lexer));

        obj.m_namedProperties[ident] = std::move(value);
        return Unit{};
    }  

    // Parse value
    auto temp = parse_next_node(lexer); reterr(temp);
    auto val = std::move(*temp);
    obj.m_positionalProperties.push_back(std::move(val));
    return Unit {};
}

Expected<std::unique_ptr<SIML::NodeObject>, SIML::ParseError> parse_next_object(SIML::Lexer& lexer) noexcept {
    
    // TODO: assert in debug mode that it is, in fact TokenType::BLOCK_OPEN
    
    lexer.consume_next();
    auto obj = std::make_unique<SIML::NodeObject>();

    while (auto token = lexer.peek()) {
        if (*token == SIML::TokenType::BLOCK_CLOSE) { // End of object
            lexer.consume_next();
            return std::move(obj);
        }
        
        auto temp = parse_object_element(lexer, *obj, *token);
        reterr(temp);
    }
    return Unexpected(SIML::ParseError("'}' expected, EOF found", lexer));
}

Expected<std::unique_ptr<SIML::NodeObject>, SIML::ParseError> SIML::NodeObject::parseAsGlobalNode(SIML::Lexer& lexer) noexcept {
    auto base_node = std::make_unique<SIML::NodeObject>();

    while (auto token = lexer.peek()) {
        auto temp = parse_object_element(lexer, *base_node, *token);
        reterr(temp);
    }

    return base_node;
}