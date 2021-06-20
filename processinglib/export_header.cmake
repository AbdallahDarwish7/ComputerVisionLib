function(export_headers TARGET HEADER_SOURCE_DIR HEADER_DEST_DIR)
    # Put all headers that are in the source directory into EXPORT_HEADERS variable
    file(GLOB_RECURSE EXPORT_HEADERS CONFIGURE_DEPENDS
            RELATIVE "${HEADER_SOURCE_DIR}"
            "${HEADER_SOURCE_DIR}/*.h"
            )

    message(${EXPORT_HEADERS})

    # For each header that will be exported
    foreach (HEADER ${EXPORT_HEADERS})

        # Get the directory portion that needs to be created
        get_filename_component(HEADER_DIRECTORY "${HEADER}" DIRECTORY)

        # Create the directory
        add_custom_command(TARGET ${TARGET} PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory "${HEADER_DEST_DIR}")
        #        message(${HEADER_DEST_DIR})
        #        message(${HEADER})

        if (MSVC) #Microsoft Visual C++

            # Make a hard link to the file
            add_custom_command(TARGET ${TARGET} PRE_BUILD
                    COMMAND if not exist "${HEADER_DEST_DIR}" \( mklink /h "${HEADER_DEST_DIR}" "${HEADER_SOURCE_DIR}/${HEADER}" \))

        else ()

            # Make a symbolic link to the file
            add_custom_command(TARGET ${TARGET} PRE_BUILD
                    COMMAND ln -sf "${HEADER_SOURCE_DIR}/${HEADER}" "${HEADER_DEST_DIR}")

        endif ()
    endforeach (HEADER)
endfunction()