#
# Building the waysome exetuable
#

#
# Set include directories
#
include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PNG_INCLUDE_DIRS}
    ${UDEV_INCLUDE_DIRS}
    ${WAYLAND_CURSOR_INCLUDE_DIRS}
    ${WAYLAND_SERVER_INCLUDE_DIRS}
)

#
# Add definitions
#
add_definitions(
    ${PNG_DEFINITIONS}
    ${UDEV_DEFINITIONS}
    ${WAYLAND_CURSOR_DEFINITIONS}
    ${WAYLAND_SERVER_DEFINITIONS}
)


#
# We use the var SOURCE_FILES to hold all source files to be linked together
# into waysome.
#
set(SOURCE_FILES
    main.c
    objects/object.c
)

#
# General compile options
#
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wunused -Wformat")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter -pedantic")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -pthread")

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

target_link_libraries(waysome
    ${PNG_LIBRARIES}
    ${UDEV_LIBRARIES}
    ${WAYLAND_CURSOR_LIBRARIES}
    ${WAYLAND_SERVER_LIBRARIES}
)


