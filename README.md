# SIML Compiler (`simlc`)

A fast, lightweight C++ reference parser and AST builder implementing the full [SIML (Simplified Interface Markup Language) Specification](https://github.com/SimplifiedInterfaceMarkupLanguage/spec).

Written in standard C++ with **zero external dependencies**.

## Building

Requires a C++ compiler with C++20/C++23 support and CMake 3.20+.

```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build library and tests
cmake --build build

# Run test suite
ctest --test-dir build --output-on-failure
```

## Quick Example

```cpp
#include <iostream>
#include "Lexer/Source.h"
#include "Lexer/Lexer.h"
#include "Node/Node.h"

int main() {
    std::string code = R"(
        .title: "Sample View";
        .enabled: true;
        
        Button {
            .width: 120;
            .height: 40;
            .label: "Submit";
        };
    )";

    SIML::Source source(code);
    SIML::Lexer lexer(source);

    auto result = SIML::NodeObject::parseAsGlobalNode(lexer);
    if (result.hasError()) {
        std::cerr << "Parsing failed: " << result.error().m_message << "\n";
        return 1;
    }

    // Access parsed AST
    std::cout << "Parsed AST successfully:\n" << **result << "\n";
    return 0;
}
```

## Specification & Documentation

For the formal grammar, AST semantics, and type rules, refer to the [SIML Specification Repository](https://github.com/SimplifiedInterfaceMarkupLanguage/spec).
