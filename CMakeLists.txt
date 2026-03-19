cmake_minimum_required(VERSION 3.10.0)
project(Qontext VERSION 0.4.0 LANGUAGES C CXX)

# Define directories
set(BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(INCLUDE_DIR ${BASE_DIR}/include)
set(SRC_DIR ${BASE_DIR}/src)

# *********************************************************************************************
#                                        CMAKE OUTPUT
# *********************************************************************************************

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BASE_DIR}/build/debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BASE_DIR}/build/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BASE_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BASE_DIR}/build/debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${BASE_DIR}/build/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${BASE_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BASE_DIR}/build/release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${BASE_DIR}/build/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${BASE_DIR}/build/lib)

# *********************************************************************************************
#                                      QONTEXT LIBRARY
# *********************************************************************************************

# Define source folders
set(INFO ${SRC_DIR}/info)
set(UTILS ${SRC_DIR}/utils)
set(STRUCTURES ${SRC_DIR}/structures)

# List of source files
set(INFO_SOURCES
    ${INFO}/endianness.cpp
)
set(UTILS_SOURCES
    ${UTILS}/timer.cpp
)
set(STRUCTURES_SOURCES

)

# List of all source files
set(SOURCES
    ${INFO_SOURCES}
    ${UTILS_SOURCES}
    ${STRUCTURES_SOURCES}
)

# Add the Qontext library
add_library(Qontext STATIC ${SOURCES})

# *********************************************************************************************
#                                  INCLUDE LOCATION DEFINITIONS
# *********************************************************************************************

set(INFO_DIR ${INCLUDE_DIR}/info)
set(UTILS_DIR ${INCLUDE_DIR}/utils)
set(STRUCTURES_DIR ${INCLUDE_DIR}/structures)

# Include directories for the Qontext library
target_include_directories(Qontext PRIVATE
    ${INCLUDE_DIR}

    ${INFO_DIR}
    ${UTILS_DIR}
    ${STRUCTURES_DIR}
    
    # external
    ${BASE_DIR}/external/qarbon/include
)

# *********************************************************************************************
#                                    QONTEXT DEBUGGING
# *********************************************************************************************

# Add the QontextDebug executable
add_executable(QontextDebug debug.cpp)

# Include directories for the QontextDebug executable
target_include_directories(QontextDebug PRIVATE
    ${INCLUDE_DIR}
    
    ${INFO_DIR}
    ${UTILS_DIR}
    ${STRUCTURES_DIR}
    
    # external
    ${BASE_DIR}/external/qarbon/include
)

# Link the Qontext library to the QontextDebug executable
target_link_libraries(QontextDebug Qontext)