#
# Command list generation setup
#
# So this is how it works:
# This directory holds "*.commands" files, through which you can add commands
# in a relatively simple way.
#
# Each line of such a file contains the command's name, a semicolon and the
# type of the command ("regular" or "special").
# For each line, a command declaration will be generated and put into a header
# with the same basename as the command name.
# The command functions declared in the generated header should be implemented
# in the corresponding source file.
#
# All the commands will be added to the generated command list in list.c,
# making them globally available through the ws_command_get() method.
#


#
# List of command files
#
set(WS_COMMAND_FILES
    arithmetical.commands
)


#
# Process all command definition files
#
foreach(COMMAND_FILE IN LISTS WS_COMMAND_FILES)
    file(STRINGS ${COMMAND_FILE} COMMANDS)
    set(COMMAND_DECLS)

    foreach(COMMAND IN LISTS COMMANDS)
        # Get command name and type
        list(GET COMMAND 0 TMP)
        string(STRIP "${TMP}" COMMAND_NAME)

        list(GET COMMAND 1 TMP)
        string(STRIP "${TMP}" COMMAND_TYPE)

        # Collect commands
        list(APPEND WS_COMMANDS "${COMMAND_NAME} ${COMMAND_TYPE}")

        # Generate command declarations
        set(COMMAND_DECLS
            "${COMMAND_DECLS}\nDECLARE_CMD_${COMMAND_TYPE}(${COMMAND_NAME});"
        )
    endforeach()

    # Generate header file for commands
    string(REPLACE ".commands" ".h" COMMAND_HEADER ${COMMAND_FILE})
    configure_file("declarations.h.in" ${COMMAND_HEADER})

    # Add to header list
    set(HEADER_INCLUSION "${HEADER_INCLUSION}\n#include \"${COMMAND_HEADER}\"")
endforeach()

#
# Sort the commands alphabetially so we can perform a binary search
#
if(DEFINED WS_COMMANDS)
    list(SORT WS_COMMANDS)
endif()

list(LENGTH WS_COMMANDS WS_COMMAND_CNT)

#
# Generate the command list source
#
set(LIST_BODY)
foreach(COMMAND IN LISTS WS_COMMANDS)
    # Get command name and type
    string(REPLACE " " ";" COMMAND_PARTS ${COMMAND})
    list(GET COMMAND_PARTS 0 COMMAND_NAME)
    list(GET COMMAND_PARTS 1 COMMAND_TYPE)

    set(LIST_BODY
        "${LIST_BODY}\n    COMMAND(${COMMAND_NAME}, ${COMMAND_TYPE})"
    )
endforeach()
configure_file("list.c.in" "list.c")

