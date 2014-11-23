#
# Building the waysome exetuable
#

#
# This variable holds all the modules
#
set(WS_MODULES
    action
    command
    compositor
    connection
    input
    logger
    objects
    protocol
    serialize
    util
    values
)

#
# Set include directories
#
include_directories(
    ${PROJECT_SOURCE_DIR}/src

    ${DRM_INCLUDE_DIRS}
    ${EV_INCLUDE_DIRS}
)

#
# Add definitions
#
add_definitions(
    ${EV_DEFINITIONS}
)

#
# We use the var SOURCE_FILES to hold all source files to be linked together
# into waysome.
#
set(SOURCE_FILES
    context.c
    main.c
)

#
# General compile options
#
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wunused -Wformat")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter -pedantic")

#
# Because those idiotic fucking morons from glibc neither ship the C11 standard
# nor the pthreads library as fallback with C11, we have to use gnu99 as C
# standard we compile with. This also prevents us from using the new features
# from C11, namingly the atomic types.
#
# With a _real_ libc, we wound't have this problem.
#
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -pthread")

if(${HARD_MODE})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wno-error=unused-function")
endif()

#
# Debugging options
#
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")


#
# Pull in all the modules
#
foreach(MODULE IN LISTS WS_MODULES)
    add_subdirectory(${MODULE})
endforeach()

#
# Add waysome executable
#
add_executable(waysome ${SOURCE_FILES})

target_link_libraries(waysome
    connection
    action
    input
    compositor
    objects
    logger

    ${EV_LIBRARIES}
)


