# - CMake script for BoostExamples Spirit tests
#
add_executable(basic_parsing basic_parsing.cpp)
add_executable(basic_parsing_istream basic_parsing_istream.cpp)

# Sehe's example of combining grammars
add_executable(sehe-multigrammar sehe-multigrammar.cpp)

# Property parser lib
add_library(PropertyParser SHARED PropertyParser.cpp)

# Data type (property parser)
add_executable(datatype-grammar datatype-grammar.cpp)
target_link_libraries(datatype-grammar PropertyParser)

