cmake_minimum_required (VERSION 3.8)
project (CSC8501_Coursework)

file(COPY resources DESTINATION ${CMAKE_BINARY_DIR} PATTERN "*.gitkeep" EXCLUDE)

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE HEAD CONFIGURE_DEPENDS "src/*.h")
add_executable (${CMAKE_PROJECT_NAME} ${SRC} ${HEAD})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()
