#ifndef SIML_SCOPE_H
#define SIML_SCOPE_H
#include "Node/Node.h"

namespace SIML
{
    struct Scope
    {
        static Scope from_node(Node& node);
    };
}
#endif //SIML_SCOPE_H
