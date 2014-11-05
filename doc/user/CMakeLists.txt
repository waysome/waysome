#
# Build user documentation
#

#
# Build the man page
#
add_custom_command(OUTPUT waysome.1
    COMMAND ${LATEX2MAN_EXECUTABLE} -M waysome.tex waysome.1
    MAIN_DEPENDENCY waysome.tex
)

if(${LATEX2MAN_FOUND})
    add_custom_target(doc ALL DEPENDS waysome.1)
endif()

