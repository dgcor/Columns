cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

# use ccache if available
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Found ccache in ${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

project(Columns VERSION 1.2.1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake_modules")

if(IS_DIRECTORY "${PROJECT_SOURCE_DIR}/../DGEngine.core")
    add_subdirectory(${PROJECT_SOURCE_DIR}/../DGEngine.core DGEngine.core)
else()
    add_subdirectory(DGEngine.core)
endif()

option(DGENGINE_COMPRESS_BINARY "Compress binary" FALSE)
option(DGENGINE_OPTIMISE "Optimize build" FALSE)
option(DGENGINE_VGM_MUSIC "Add support for VGM sound files" TRUE)

find_package(gme QUIET)
find_package(ZLIB QUIET)

if(NOT ${gme_FOUND} OR NOT ${ZLIB_FOUND})
    SET(DGENGINE_VGM_MUSIC FALSE)
endif()

set(SOURCE_FILES
    src/RegisterHooks.cpp
    src/RegisterHooks.h
    src/Game/Game2.cpp
    src/Game/Game2.h
    src/Game/Actions/ActLevel.h
    src/Game/Jewel/Jewel.cpp
    src/Game/Jewel/Jewel.h
    src/Game/Jewel/JewelClass.h
    src/Game/Jewel/Jewels.cpp
    src/Game/Jewel/Jewels.h
    src/Game/Level/Level.cpp
    src/Game/Level/Level.h
    src/Game/Level/LevelBoard.cpp
    src/Game/Level/LevelBoard.h
    src/Game/Level/LevelBoardUtils.cpp
    src/Game/Level/LevelBoardUtils.h
    src/Game/Level/LevelCell.h
    src/Game/Level/LevelDraw.cpp
    src/Game/Level/LevelDraw.h
    src/Game/Level/LevelInputManager.cpp
    src/Game/Level/LevelInputManager.h
    src/Game/Level/LevelOptions.h
    src/Game/Level/LevelOptionsManager.cpp
    src/Game/Level/LevelOptionsManager.h
    src/Game/Level/LevelPlayer.cpp
    src/Game/Level/LevelPlayer.h
    src/Game/Level/LevelPlayerManager.cpp
    src/Game/Level/LevelPlayerManager.h
    src/Game/Level/LevelSave.cpp
    src/Game/Level/LevelSave.h
    src/Game/Level/LevelSaveObject.h
    src/Game/Level/LevelScore.h
    src/Game/Level/LevelScoreManager.cpp
    src/Game/Level/LevelScoreManager.h
    src/Game/Level/LevelState.h
    src/Game/Level/LevelStateObject.h
    src/Game/Level/LevelUIObject.cpp
    src/Game/Level/LevelUIObject.h
    src/Parser/ParseAction2.cpp
    src/Parser/ParseAction2.h
    src/Parser/ParseFile2.cpp
    src/Parser/ParseFile2.h
    src/Parser/Actions/ParseLevelActions.cpp
    src/Parser/Actions/ParseLevelActions.h
    src/Parser/Jewel/ParseJewel.cpp
    src/Parser/Jewel/ParseJewel.h
    src/Parser/Jewel/ParseJewelClass.cpp
    src/Parser/Jewel/ParseJewelClass.h
    src/Parser/Level/ParseLevel.cpp
    src/Parser/Level/ParseLevel.h
    src/Parser/Level/ParseLevelOptions.cpp
    src/Parser/Level/ParseLevelOptions.h
    src/Parser/Level/ParseLevelSave.cpp
    src/Parser/Level/ParseLevelSave.h
    src/Parser/Level/ParseLevelScores.cpp
    src/Parser/Level/ParseLevelScores.h
)

if(DGENGINE_VGM_MUSIC)
    SET(SOURCE_FILES ${SOURCE_FILES}
        src/SFML/VGM.cpp
        src/SFML/VGM.h
    )
endif()

if(DGENGINE_STATIC_CRT AND WIN32)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

add_library(${PROJECT_NAME}Lib OBJECT ${SOURCE_FILES})

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${PROJECT_NAME}Lib PUBLIC -Wall -stdlib=libc++)
    target_link_options(${PROJECT_NAME}Lib PUBLIC -stdlib=libc++)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11.1)
        message(FATAL_ERROR "GCC version must be at least 11.1!")
    endif()
    target_compile_options(${PROJECT_NAME}Lib PUBLIC -Wall -Wpedantic)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${PROJECT_NAME}Lib PUBLIC /W3 /wd4250 /wd4996)
endif()

target_include_directories(${PROJECT_NAME}Lib PUBLIC src)
target_link_libraries(${PROJECT_NAME}Lib PUBLIC DGEngine.core)

if(DGENGINE_VGM_MUSIC)
    target_compile_definitions(${PROJECT_NAME}Lib PUBLIC DGENGINE_VGM_MUSIC=1)
    target_link_libraries(${PROJECT_NAME}Lib PRIVATE gme::gme ZLIB::ZLIB)
endif()

add_executable(${PROJECT_NAME} src/Main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}Lib)

if(DGENGINE_OPTIMISE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_options(${PROJECT_NAME} PRIVATE -O3)
        set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE -s)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        target_compile_options(${PROJECT_NAME} PRIVATE /GF /EHsc)
        target_link_options(${PROJECT_NAME} PRIVATE /OPT:ICF /OPT:REF)
    endif()
    if(DGENGINE_COMPRESS_BINARY)
        include(FindSelfPackers)
        if(SELF_PACKER_FOR_EXECUTABLE)
            ADD_CUSTOM_COMMAND(
                COMMAND ${SELF_PACKER_FOR_EXECUTABLE} $<TARGET_FILE:${PROJECT_NAME}>
                ARGS ${SELF_PACKER_FOR_EXECUTABLE_FLAGS} -9q
                TARGET ${PROJECT_NAME}
                POST_BUILD
            )
        endif()
    endif()
endif()

# create package

install(TARGETS ${PROJECT_NAME} DESTINATION .)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/gamefilesc" DESTINATION .)
install(FILES
    "${PROJECT_SOURCE_DIR}/LICENSE.txt"
    "${PROJECT_SOURCE_DIR}/LICENSE.Zlib.txt"
    "${PROJECT_SOURCE_DIR}/main.json"
    "${PROJECT_SOURCE_DIR}/README.md"
    DESTINATION .)

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.Zlib.txt")

include(CPack)
