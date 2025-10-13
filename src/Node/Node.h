#ifndef SIML_NODE_H
#define SIML_NODE_H

#include "../Lexer/Lexer.h"
#include "../Expected.h"
#include <iostream>
#include <string_view>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>
#include <iostream>

namespace SIML {
    enum class NodeType {
        STRING,
        NUMBER,
        OBJECT,
        COMPONENT,
        IDENT
    };

    struct NodeString;
    struct NodeNumber;
    struct NodeObject;
    struct NodeComponent;
    struct NodeIdent;

    struct Node {
        explicit Node(const NodeType type) noexcept : m_nodeType(type) {}

        NodeType m_nodeType;
        virtual ~Node() = default;
        [[nodiscard]] const NodeString* tryCastString() const noexcept;
        NodeString* tryCastString() noexcept;
        [[nodiscard]] const NodeNumber* tryCastNumber() const noexcept;
        NodeNumber* tryCastNumber() noexcept;
        [[nodiscard]] const NodeObject* tryCastObject() const noexcept;
        NodeObject* tryCastObject() noexcept;
        [[nodiscard]] const NodeComponent* tryCastComponent() const noexcept;
        NodeComponent* tryCastComponent() noexcept;
        [[nodiscard]] const NodeIdent* tryCastIdent() const noexcept;
        NodeIdent* tryCastIdent() noexcept;

        static std::string nodeTypeToString(const NodeType nodeType)
        {
            switch (nodeType) {
                case SIML::NodeType::STRING: return "String";
                case SIML::NodeType::COMPONENT: return "Component";
                case SIML::NodeType::IDENT: return "Ident";
                case SIML::NodeType::NUMBER: return "Number";
                case SIML::NodeType::OBJECT: return "Object";
            }
            return "Unknown";
        }

        friend std::ostream& operator<<(std::ostream &os, const Node &node) {
            return writeInternal(os, node, 0);
        }

    private:
        static std::ostream& writeInternal(std::ostream& os, const SIML::Node& node, int spacing);
    };

    struct NodeString : Node {
        NodeString() noexcept : Node(NodeType::STRING) {}\
        ~NodeString() override = default;

        std::string_view m_unescapedValue;
        std::optional<std::string_view> tag;

        [[nodiscard]] std::string escaped() const noexcept;
    };

    struct NodeNumber : Node {
        NodeNumber() noexcept : Node(NodeType::NUMBER) {}
        ~NodeNumber() override = default;
        
        std::optional<std::string_view> m_rawIntegerPart;
        std::optional<std::optional<std::string_view>> m_rawFloatPart;
        std::optional<std::string_view> m_tag;

        [[nodiscard]] bool isFloat() const noexcept;
        [[nodiscard]] int integerPart() const noexcept;
        [[nodiscard]] int floatPart() const noexcept;
        [[nodiscard]] float asFloat() const noexcept;
        [[nodiscard]] double asDouble() const noexcept;
    };

    struct NodeObject : Node {
        NodeObject() noexcept : Node(NodeType::OBJECT) {}
        ~NodeObject() override = default;

        std::unordered_map<std::string_view, std::unique_ptr<Node>> m_namedProperties;
        std::vector<std::unique_ptr<Node>> m_positionalProperties;
        
        static Expected<std::unique_ptr<NodeObject>, ParseError> parseAsGlobalNode(Lexer& lexer) noexcept;
    };

    struct NodeComponent : Node {
        NodeComponent() noexcept : Node(NodeType::COMPONENT) {}
        ~NodeComponent() override = default;

        std::string_view m_name;
        std::unique_ptr<Node> m_value;
    };

    struct NodeIdent : Node {
        NodeIdent() noexcept : Node(NodeType::IDENT) {}
        ~NodeIdent() override = default;

        std::string_view ident;
    };

#define tryCast(name, type) \
    inline [[nodiscard]] const Node##name* Node::tryCast##name() const noexcept \
    { return m_nodeType == NodeType::type ? static_cast<const Node##name *>(this) : nullptr; } \
    inline [[nodiscard]] Node##name* Node::tryCast##name() noexcept \
    { return m_nodeType == NodeType::type ? static_cast<Node##name *>(this) : nullptr; } \

    tryCast(String, STRING)
    tryCast(Number, NUMBER)
    tryCast(Object, OBJECT)
    tryCast(Component, COMPONENT)
    tryCast(Ident, IDENT)
}

#endif