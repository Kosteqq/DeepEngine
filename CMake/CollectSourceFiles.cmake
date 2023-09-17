set(SOURCE_FILES "")

# Get all files from subdirectories
file(GLOB_RECURSE SUBDIRECTORIES "${SOURCE_DIR}/*")
foreach(FILENAME ${SUBDIRECTORIES})
    string(FIND "${FILENAME}" "3rdParty" FOUND_INDEX)

    if(FOUND_INDEX GREATER -1)
        continue()
    endif()

    if(${FILENAME} MATCHES ".*.cpp")
        message("Found CPP File: ${FILENAME}")
        list(APPEND SOURCE_FILES ${FILENAME})
        continue()
    endif()

   if(${FILENAME} MATCHES ".*.h")
       message("Found Header File: ${FILENAME}")
       list(APPEND SOURCE_FILES ${FILENAME})
        continue()
  endif()
endforeach()