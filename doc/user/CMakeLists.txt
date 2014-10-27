#
# Build user documentation
#

#
# Build the info page
#
add_custom_command(OUTPUT waysome.texi
    COMMAND ${LATEX2MAN_EXECUTABLE} -T waysome.tex waysome.texi
    MAIN_DEPENDENCY waysome.tex
)

add_custom_command(OUTPUT waysome.info
        COMMAND ${MAKEINFO_EXECUTABLE} waysome.texi
    MAIN_DEPENDENCY waysome.texi
)

#
# Build the man page
#
add_custom_command(OUTPUT waysome.1
    COMMAND ${LATEX2MAN_EXECUTABLE} -M waysome.tex waysome.1
    MAIN_DEPENDENCY waysome.tex
)

if(${LATEX2MAN_FOUND} AND ${MAKEINFO_FOUND})
    add_custom_target(doc ALL DEPENDS waysome.info waysome.1)
endif()

