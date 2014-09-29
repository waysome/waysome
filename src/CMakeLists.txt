#
# Building the waysome exetuable
#

include_directories(${PROJECT_SOURCE_DIR}/src)


#
# We use the var SOURCE_FILES to hold all source files to be linked together
# into waysome.
#
set(SOURCE_FILES
    main.c
)

#
# General compile options
#
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wunused -Wformat")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter -pedantic")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")

if(${HARD_MODE})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wno-error=unused-function")
endif()

#
# Debugging options
#
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")

#
# Libreset will be a shared object
#
add_executable(waysome ${SOURCE_FILES})


