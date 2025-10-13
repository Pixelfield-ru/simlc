#include "./Node.h"
#include "../Expected.h"
#include "../Node/Node.h"
#include <charconv>
#include <exception>
#include <optional>
#include <string>
#include <vector>

void repeat(std::ostream& os, char character, int amount)
{
    for (int i = 0; i < amount; i++) {
        os << character;
    }
}

std::ostream& SIML::Node::writeInternal(std::ostream& os, const SIML::Node& node, int spacing) {
    if (const auto str = node.tryCastString()) {
        os << "\"" << str->m_unescapedValue << "\"" << ";";
    } else if (const auto number = node.tryCastNumber()) {
        os << number->asDouble() << ";"; // TODO: fixme to string representation
    } else if (const auto ident = node.tryCastIdent()) {
        os << ident << ";";
    } else if (const auto object = node.tryCastObject()) {
        os << "{" << "\n";
        const auto newSpacing = spacing + 4;
        for (auto& [name, prop] : object->m_namedProperties) {
            repeat(os, ' ', newSpacing);
            os << "." << name << ": ";
            writeInternal(os, *prop, newSpacing);
            os << "\n";
        }
        for (auto& prop : object->m_positionalProperties) {
            repeat(os, ' ', newSpacing);
            writeInternal(os, *prop, newSpacing);
            os << "\n";
        }
    } else if (const auto component = node.tryCastComponent()) {
        os << component->m_name;
        writeInternal(os, *component->m_value, spacing);
    }

    return os;
}


std::string SIML::NodeString::escaped() const noexcept {
    auto chars = std::vector<char>();
    for (auto c : m_unescapedValue) {
        if (c != '\\') {
            chars.push_back(c);
        }
    }

    return {chars.begin(), chars.end()};
}

bool SIML::NodeNumber::isFloat() const noexcept {
    return m_rawFloatPart != std::nullopt;
}

int parse_str(const std::string_view str) noexcept {
    try {
        int i;
        std::from_chars(&*str.begin(), &*str.end(), i);
        return i;
    } catch (const std::exception& e) {}
    return 0; // How tf did you get there???
}

int SIML::NodeNumber::integerPart() const noexcept {
    if (!m_rawFloatPart) {
        return 0;
    }
    return parse_str(*m_rawIntegerPart);
}

int SIML::NodeNumber::floatPart() const noexcept {
    if (!m_rawFloatPart && !(*m_rawFloatPart)) {
        return 0;
    }
    return parse_str(**m_rawFloatPart);
}

float SIML::NodeNumber::asFloat() const noexcept {
    float total = integerPart();
    if (m_rawFloatPart && *m_rawFloatPart) {
        total += (float)floatPart() / ((**m_rawFloatPart).length() * 10);
    }

    return total;
}

double SIML::NodeNumber::asDouble() const noexcept {
    double total = integerPart();
    if (m_rawFloatPart && *m_rawFloatPart) {
        total += (double)floatPart() / ((**m_rawFloatPart).length() * 10);
    }

    return total;
}