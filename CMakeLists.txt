cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

# use ccache if available
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Found ccache in ${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

project(Columns VERSION 1.2.0)

if(IS_DIRECTORY "${PROJECT_SOURCE_DIR}/../DGEngine.core")
    add_subdirectory(${PROJECT_SOURCE_DIR}/../DGEngine.core DGEngine.core)
else()
    add_subdirectory(DGEngine.core)
endif()

option(DGENGINE_COMPRESS_BINARY "Compress binary" FALSE)
option(DGENGINE_OPTIMISE "Optimize build" FALSE)

set(SOURCE_FILES
    src/RegisterHooks.cpp
    src/RegisterHooks.h
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

if(DGENGINE_STATIC_CRT AND WIN32)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

add_library(${PROJECT_NAME}Lib ${SOURCE_FILES})

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
            )
        endif()
    endif()
endif()
