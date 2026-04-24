# Auto-initialize git submodules if needed
# This is useful when the repo was cloned without --recursive

function(init_git_submodules)
    find_package(Git QUIET)

    if(NOT GIT_FOUND)
        message(WARNING "Git not found, cannot auto-initialize submodules")
        return()
    endif()

    # Check if we're in a git repository
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --is-inside-work-tree
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE IS_GIT_REPO
        OUTPUT_QUIET
        ERROR_QUIET
    )

    if(NOT IS_GIT_REPO EQUAL 0)
        message(STATUS "Not a git repository, skipping submodule initialization")
        return()
    endif()

    # Check if .gitmodules exists
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/.gitmodules")
        message(STATUS "No .gitmodules file found, skipping submodule initialization")
        return()
    endif()

    # Check if submodules need initialization by checking if any submodule path is empty
    # Read .gitmodules to get submodule paths
    file(READ "${CMAKE_SOURCE_DIR}/.gitmodules" GITMODULES_CONTENT)

    # Parse submodule paths from .gitmodules
    string(REGEX MATCHALL "path[ \t]*=[ \t]*([^\n]+)" SUBMODULE_PATHS "${GITMODULES_CONTENT}")

    set(NEEDS_INIT FALSE)
    foreach(MATCH ${SUBMODULE_PATHS})
        string(REGEX REPLACE "path[ \t]*=[ \t]*" "" SUBMODULE_PATH "${MATCH}")
        string(STRIP "${SUBMODULE_PATH}" SUBMODULE_PATH)

        # Check if the submodule directory exists and has content
        set(SUBMODULE_FULL_PATH "${CMAKE_SOURCE_DIR}/${SUBMODULE_PATH}")
        if(NOT EXISTS "${SUBMODULE_FULL_PATH}/.git")
            # Check if directory is empty or missing
            if(NOT EXISTS "${SUBMODULE_FULL_PATH}")
                message(STATUS "Submodule path missing: ${SUBMODULE_PATH}")
                set(NEEDS_INIT TRUE)
            elseif(NOT EXISTS "${SUBMODULE_FULL_PATH}/CMakeLists.txt" AND NOT EXISTS "${SUBMODULE_FULL_PATH}/README.md")
                # Directory exists but appears empty (no typical project files)
                message(STATUS "Submodule appears uninitialized: ${SUBMODULE_PATH}")
                set(NEEDS_INIT TRUE)
            endif()
        endif()
    endforeach()

    if(NEEDS_INIT)
        message(STATUS "Initializing git submodules...")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            RESULT_VARIABLE GIT_SUBMODULE_RESULT
            OUTPUT_VARIABLE GIT_SUBMODULE_OUTPUT
            ERROR_VARIABLE GIT_SUBMODULE_ERROR
        )

        if(GIT_SUBMODULE_RESULT EQUAL 0)
            message(STATUS "Git submodules initialized successfully")
        else()
            message(WARNING "Failed to initialize git submodules:\n${GIT_SUBMODULE_ERROR}")
        endif()
    endif()
endfunction()
