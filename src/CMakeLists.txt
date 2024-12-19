add_library(Buffalo INTERFACE
        grammar/GrammarBuilder.h)

target_sources(Buffalo PUBLIC
        lr/LRTableSerializer.cpp
        lr/LRTableSerializer.h
        helpers/TupleUtils.h
        RecursiveFunctionsGrammar.cpp
        RecursiveFunctionsGrammar.h

        SyntaxNode.h
        grammar/BuildersRegistry.h
        lr/Position.h
        lr/TokensBitset.h
        helpers/SmartEnum.h
        lr/LRTableSerializer.cpp

        lexis/LexicalAnalyzer.cpp
        lexis/LexicalAnalyzer.h

        lr/TransitiveClosure.h
        lr/LRTableBuilder.cpp
        lr/LRTableBuilder.h
        grammar/Grammar.cpp
        grammar/Grammar.h
        lr/LRParser.h
        grammar/GrammarProduction.h
        helpers/Hashers.h
)

target_include_directories(Buffalo INTERFACE . lr grammar helpers)
