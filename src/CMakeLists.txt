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
    ${EV_INCLUDE_DIRS}
    ${ICU_OP_INCLUDE_DIRS}
    ${ICU_UC_INCLUDE_DIRS}
    ${libreset_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
    ${UDEV_INCLUDE_DIRS}
    ${WAYLAND_CURSOR_INCLUDE_DIRS}
    ${WAYLAND_SERVER_INCLUDE_DIRS}
    ${YAJL_INCLUDE_DIRS}
)

#
# Add definitions
#
add_definitions(
    ${DRM_DEFINITIONS}
    ${EGL_DEFINITIONS}
    ${EV_DEFINITIONS}
    ${ICU_OP_DEFINITIONS}
    ${ICU_UC_DEFINITIONS}
    ${libreset_DEFINITIONS}
    ${PNG_DEFINITIONS}
    ${UDEV_DEFINITIONS}
    ${WAYLAND_CURSOR_DEFINITIONS}
    ${WAYLAND_SERVER_DEFINITIONS}
    ${YAJL_DEFINITIONS}
)


#
# We use the var SOURCE_FILES to hold all source files to be linked together
# into waysome.
#
set(SOURCE_FILES
    compositor/background_surface.c
    compositor/module.c
    logger/module.c
    main.c
    objects/array.c
    objects/object.c
    objects/set.c
    objects/wayland_obj.c
    util/cleaner.c
    util/wayland.c
    values/bool.c
    values/int.c
    values/nil.c
    values/value.c
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
# Libreset will be a shared object
#
add_executable(waysome ${SOURCE_FILES})

target_link_libraries(waysome
    ${DRM_LIBRARIES}
    ${EGL_LIBRARIES}
    ${EV_LIBRARIES}
    ${libreset_LIBRARIES}
    ${PNG_LIBRARIES}
    ${UDEV_LIBRARIES}
    ${WAYLAND_CURSOR_LIBRARIES}
    ${WAYLAND_SERVER_LIBRARIES}
    ${YAJL_LIBRARIES}

    ${ICU_OP_LIBRARIES}
    ${ICU_UC_LIBRARIES}
)


