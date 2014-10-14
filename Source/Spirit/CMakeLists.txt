# - CMake script for BoostExamples Spirit tests
#

# Simple recursive data structure
add_executable(rds_parsing rds_parsing.cpp)

#-----------------------------------------------------------------------
# Refined stuff...

# Property parser lib
add_library(PropertyParser SHARED
  Property.hpp
  BitsetGrammar.hpp
  PropertyParser.hpp
  PropertyParser.cpp
  )

# PropertyChecker app
add_executable(PropertyChecker
  PropertyChecker.cpp
  PropertyCheckerInterfaces.hpp
  PropertyCheckerInterfaces.cpp
  )
target_link_libraries(PropertyChecker PropertyParser)

set_target_properties(PropertyChecker PropertyParser
  PROPERTIES FOLDER "Spirit"
  )

