add_library(Buffalo INTERFACE)

target_sources(Buffalo PUBLIC
        Grammar.cpp
        Grammar.h
        Parser.h

        earley/EarleyParser.cpp
        earley/EarleyParser.h
)

target_include_directories(Buffalo INTERFACE . earley)
