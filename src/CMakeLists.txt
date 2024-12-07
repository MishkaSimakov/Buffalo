add_library(Buffalo INTERFACE
        helpers/TupleHasher.h)

target_sources(Buffalo PUBLIC
        grammar/Grammar.cpp
        grammar/Grammar.h
        Parser.h

        earley/EarleyParser.cpp
        earley/EarleyParser.h
        grammar/GrammarProduction.h
        grammar/GrammarBuilder.cpp
        grammar/GrammarBuilder.h
)

target_include_directories(Buffalo INTERFACE . earley grammar helpers)
