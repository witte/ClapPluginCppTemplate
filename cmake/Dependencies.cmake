find_package(clap CONFIG)

if (NOT clap_FOUND)
    message("Using FetchContent to donwload 'clap' and 'clap-helpers'")

    include(FetchContent)
    FetchContent_Declare(
        clap
        GIT_REPOSITORY https://github.com/free-audio/clap.git
        GIT_TAG main
        FIND_PACKAGE_ARGS NAMES clap
    )

    # Installed clap-helpers doesn't have a cmake target, so for now
    # I'm treating 'clap' and 'clap-helpers' as the same thing
    FetchContent_Declare(
        clap-helpers
        GIT_REPOSITORY https://github.com/free-audio/clap-helpers.git
        GIT_TAG main
        FIND_PACKAGE_ARGS NAMES clap-helpers
    )

    # Remove this whenever clap-helpers has a cmake target
    include_directories(SYSTEM ${FETCHCONTENT_BASE_DIR}/clap-helpers-src/include)

    FetchContent_MakeAvailable(clap clap-helpers)
endif()
