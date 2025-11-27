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

    template <typename From, typename To>
    concept CanBeCasted = requires(From* from) {
        {To::tryCastFrom(from)} -> std::convertible_to<To*>;
    };

    struct Node {
        explicit Node(const NodeType type) noexcept : m_nodeType(type) {}

        NodeType m_nodeType;
        virtual ~Node() = default;

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

        template<typename T> requires CanBeCasted<Node, T>
        [[nodiscard]] const T* tryCastInto() const noexcept {
            return T::tryCastFrom(this);
        }

        template<typename T> requires CanBeCasted<Node, T>
        [[nodiscard]] T* tryCastInto() noexcept {
            return T::tryCastFrom(this);
        }
    private:
        static std::ostream& writeInternal(std::ostream& os, const SIML::Node& node, int spacing);
    };

#define tryCastFromDef(nodeName, nodeType) \
[[nodiscard]] static nodeName* tryCastFrom(Node* node) noexcept { \
    return node->m_nodeType == NodeType::nodeType ? \
    static_cast<nodeName*>(node) : nullptr; \
} \
[[nodiscard]] static const nodeName* tryCastFrom(const Node* node) noexcept { \
    return node->m_nodeType == NodeType::nodeType ? \
    static_cast<const nodeName*>(node) : nullptr; \
}

    struct NodeString : Node {
        NodeString() noexcept : Node(NodeType::STRING) {}\
        ~NodeString() override = default;

        std::string_view m_unescapedValue;
        std::optional<std::string_view> tag;

        [[nodiscard]] std::string escaped() const noexcept;

        tryCastFromDef(NodeString, STRING)
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

        tryCastFromDef(NodeNumber, NUMBER)
    };

    struct NodeObject : Node {
        NodeObject() noexcept : Node(NodeType::OBJECT) {}
        ~NodeObject() override = default;

        std::unordered_map<std::string_view, std::unique_ptr<Node>> m_namedProperties;
        std::vector<std::unique_ptr<Node>> m_positionalProperties;
        
        static Expected<std::unique_ptr<NodeObject>, ParseError> parseAsGlobalNode(Lexer& lexer) noexcept;

        tryCastFromDef(NodeObject, OBJECT)
    };

    struct NodeComponent : Node {
        NodeComponent() noexcept : Node(NodeType::COMPONENT) {}
        ~NodeComponent() override = default;

        std::string_view m_name;
        std::unique_ptr<Node> m_value;

        tryCastFromDef(NodeComponent, COMPONENT)
    };

    struct NodeIdent : Node {
        NodeIdent() noexcept : Node(NodeType::IDENT) {}
        ~NodeIdent() override = default;

        std::string_view ident;

        tryCastFromDef(NodeIdent, IDENT)
    };
}

#endif