find_path(gme_INCLUDE_DIR NAMES gme.h gme/gme.h)
mark_as_advanced(gme_INCLUDE_DIR)

find_library(gme_LIBRARY NAMES gme)
mark_as_advanced(gme_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(gme REQUIRED_VARS
    gme_INCLUDE_DIR
    gme_LIBRARY
)

if(gme_FOUND)
    set(gme_INCLUDE_DIRS ${gme_INCLUDE_DIR})
    set(gme_LIBRARIES ${gme_LIBRARY})
    if(NOT TARGET gme::gme)
        add_library(gme::gme UNKNOWN IMPORTED)
        set_target_properties(gme::gme PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES "C"
            IMPORTED_LOCATION "${gme_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${gme_INCLUDE_DIR}")
    endif()
endif()
