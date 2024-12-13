include(GoogleTest)

# adding unit tests
add_executable(
        Tests
        unit/TestGrammars.cpp
        unit/LRParserTests.cpp
        unit/TestGrammars.h
        unit/GrammarBuilderTests.cpp
        unit/GrammarTests.cpp
        unit/LRTableBuilderTests.cpp
)

target_link_libraries(Tests PUBLIC GTest::gtest_main fmt::fmt Buffalo)
target_include_directories(Tests PUBLIC .)
gtest_discover_tests(Tests)
