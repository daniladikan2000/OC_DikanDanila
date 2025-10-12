add_library(NumberLib STATIC Number.cpp Number.h)

set_target_properties(NumberLib PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(NumberLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
