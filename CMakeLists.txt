cmake_minimum_required(VERSION 3.12.4 FATAL_ERROR)

project(Columns)

add_subdirectory(DGEngine.core)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
    add_definitions(-Wall -stdlib=libc++)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11.0)
        message(FATAL_ERROR "GCC version must be at least 11!")
    endif()
    if(NOT BEOS)
        add_definitions(-Wall)
    endif()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules")

if(MOVIE_SUPPORT)
    find_package(FFmpeg COMPONENTS avcodec avformat avutil swscale)
endif()
find_package(PhysFS REQUIRED)
find_package(SFML 2.5 REQUIRED system window graphics network audio)

include_directories(src)

set(SOURCE_FILES
    src/Main.cpp
    src/RegisterHooks.cpp
    src/RegisterHooks.h
    src/Actions/ActLevel.h
    src/Game/Jewel.cpp
    src/Game/Jewel.h
    src/Game/JewelClass.cpp
    src/Game/JewelClass.h
    src/Game/Level.cpp
    src/Game/Level.h
    src/Parser/ParseAction2.cpp
    src/Parser/ParseAction2.h
    src/Parser/ParseFile2.cpp
    src/Parser/ParseFile2.h
    src/Parser/ParseJewel.cpp
    src/Parser/ParseJewel.h
    src/Parser/ParseJewelClass.cpp
    src/Parser/ParseJewelClass.h
    src/Parser/ParseLevel.cpp
    src/Parser/ParseLevel.h
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

if(FFmpeg_FOUND)
    include_directories(${FFmpeg_INCLUDES})
    target_link_libraries(${PROJECT_NAME} ${FFmpeg_LIBRARIES})
endif()

include_directories(${PHYSFS_INCLUDE_DIRS} ${SFML_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${PHYSFS_LIBRARY} ${SFML_LIBRARIES})

target_link_libraries(${PROJECT_NAME} DGEngine.core)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
