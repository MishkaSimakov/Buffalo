add_library(Buffalo INTERFACE)

target_sources(Buffalo PUBLIC
        lr/TransitiveClosure.h
        lr/LRTableBuilder.cpp
        lr/LRTableBuilder.h
        grammar/Grammar.cpp
        grammar/Grammar.h
        Parser.h
        lr/LRParser.cpp
        lr/LRParser.h
        grammar/GrammarProduction.h
        grammar/GrammarBuilder.cpp
        grammar/GrammarBuilder.h
        helpers/Hashers.h
)

target_include_directories(Buffalo INTERFACE . lr grammar helpers)
