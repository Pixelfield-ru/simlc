#ifndef SIML_PLAINVIEW_H
#define SIML_PLAINVIEW_H
#include "Scope.h"
#include "Node/Node.h"

namespace SIML
{
    template<typename NodeType> requires requires(NodeType node) {{node} -> std::convertible_to<NodeType>;}
    struct PlainView
    {
        NodeType& m_currentNode;
        std::unique_ptr<Scope> m_scope;

        explicit PlainView(NodeType& node) :
            m_currentNode(node),
            m_scope(std::make_unique<Scope>(Scope::from_node(node)))
        {};
    };
}

#endif //SIML_PLAINVIEW_H
