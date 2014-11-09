#
# Build user documentation
#

#
# List holding all the tex files relevant for every version of the
# documentation
#
set(TEX_FILES
    waysome.tex
)

set(MANPAGE_TEX ${CMAKE_CURRENT_SOURCE_DIR}/manpage.tex)


#
# contrary to other compilers, pdflatex doesn't like the sources to be in
# another directory
#
if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})
    set(BUILD_TEX_FILES ${TEX_FILES})
else()
    foreach(TEX_FILE IN LISTS TEX_FILES)
        set(DEST_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TEX_FILE})
        # file(COPY) doesn't support file destinations and dirname would be
        # more work...
        add_custom_command(OUTPUT ${TEX_FILE}
                COMMAND cp -Tu ${CMAKE_CURRENT_SOURCE_DIR}/${TEX_FILE}
                    ${DEST_FILE}
            DEPENDS ${TEX_FILE}
        )
        set(BUILD_TEX_FILES ${BUILD_TEX_FILES} ${DEST_FILE})
    endforeach()
endif ()


#
# Build the PDF version of the documentation
#
add_custom_command(OUTPUT waysome.pdf
    COMMAND ${PDFLATEX_COMPILER} -halt-on-error waysome
    DEPENDS ${BUILD_TEX_FILES}
)

#
# Build the man page
#
add_custom_command(OUTPUT waysome.1
    COMMAND ${LATEX2MAN_EXECUTABLE} -M ${MANPAGE_TEX} waysome.1
    MAIN_DEPENDENCY ${MANPAGE_TEX}
    DEPENDS ${BUILD_TEX_FILES}
)

if(${LATEX2MAN_FOUND} AND PDFLATEX_COMPILER)
    add_custom_target(doc ALL DEPENDS waysome.1 waysome.pdf)
endif()

