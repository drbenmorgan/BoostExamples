# - CMake script for BoostExamples Spirit tests
#

#-----------------------------------------------------------------------
# Refined stuff...

# Property parser lib
add_library(PropertyParser SHARED
  BitsetGrammar.hpp
  Property.hpp
  PropertyGrammar.hpp
  PropertyParser.hpp
  PropertyParser.cpp
  )
target_link_libraries(PropertyParser PUBLIC Boost::boost)

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


add_executable(testIdentifier testIdentifier.cpp)
target_link_libraries(testIdentifier catch-main)
add_test(NAME testIdentifier COMMAND testIdentifier)
