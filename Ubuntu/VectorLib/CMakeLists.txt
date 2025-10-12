add_library(VectorLib SHARED Vector.cpp Vector.h SharedLib_API.h)
target_link_libraries(VectorLib PUBLIC NumberLib)
