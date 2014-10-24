#
# Building the waysome exetuable
#


#
# Set include directories
#
include_directories(
    ${PROJECT_SOURCE_DIR}/src

    ${DRM_INCLUDE_DIRS}
    ${EGL_INCLUDE_DIRS}
    ${EVDEV_INCLUDE_DIRS}
    ${EV_INCLUDE_DIRS}
    ${ICU_OP_INCLUDE_DIRS}
    ${ICU_UC_INCLUDE_DIRS}
    ${libreset_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
    ${UDEV_INCLUDE_DIRS}
    ${WAYLAND_CURSOR_INCLUDE_DIRS}
    ${WAYLAND_SERVER_INCLUDE_DIRS}
    ${XKB_COMMON_INCLUDE_DIRS}
    ${YAJL_INCLUDE_DIRS}
    ${UUID_INCLUDE_DIRS}
)

#
# Add definitions
#
add_definitions(
    ${DRM_DEFINITIONS}
    ${EGL_DEFINITIONS}
    ${EVDEV_DEFINITIONS}
    ${EV_DEFINITIONS}
    ${ICU_OP_DEFINITIONS}
    ${ICU_UC_DEFINITIONS}
    ${libreset_DEFINITIONS}
    ${PNG_DEFINITIONS}
    ${UDEV_DEFINITIONS}
    ${WAYLAND_CURSOR_DEFINITIONS}
    ${WAYLAND_SERVER_DEFINITIONS}
    ${XKB_COMMON_DEFINITIONS}
    ${YAJL_DEFINITIONS}
    ${UUID_DEFINITIONS}
)


#
# We use the var SOURCE_FILES to hold all source files to be linked together
# into waysome.
#
set(SOURCE_FILES
    $<TARGET_OBJECTS:action>
    $<TARGET_OBJECTS:command>
    $<TARGET_OBJECTS:compositor>
    $<TARGET_OBJECTS:connection>
    $<TARGET_OBJECTS:input>
    $<TARGET_OBJECTS:logger>
    $<TARGET_OBJECTS:objects>
    main.c
    serialize/deserializer.c
    serialize/json/common.c
    serialize/json/deserializer.c
    serialize/json/deserializer_state.c
    serialize/json/string_jump_state.c
    serialize/serializer.c
    util/cleaner.c
    util/wayland.c
    values/bool.c
    values/int.c
    values/nil.c
    values/string.c
    values/union.c
    values/utils.c
    values/value.c
    values/value_named.c
    values/value_type.c
    values/set.c
    values/object_id.c
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
add_subdirectory(action)
add_subdirectory(command)
add_subdirectory(compositor)
add_subdirectory(connection)
add_subdirectory(input)
add_subdirectory(logger)
add_subdirectory(objects)

#
# Add waysome executable
#
add_executable(waysome ${SOURCE_FILES})

target_link_libraries(waysome
    ${DRM_LIBRARIES}
    ${EGL_LIBRARIES}
    ${EVDEV_LIBRARIES}
    ${EV_LIBRARIES}
    ${libreset_LIBRARIES}
    ${PNG_LIBRARIES}
    ${UDEV_LIBRARIES}
    ${WAYLAND_CURSOR_LIBRARIES}
    ${WAYLAND_SERVER_LIBRARIES}
    ${XKB_COMMON_LIBRARIES}
    ${YAJL_LIBRARIES}
    ${UUID_LIBRARIES}

    ${ICU_OP_LIBRARIES}
    ${ICU_UC_LIBRARIES}
)


