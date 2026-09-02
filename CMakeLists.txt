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
set(MSDF_DIR ${SRC_DIR}/msdf)
set(INFO_DIR ${SRC_DIR}/info)
set(UTILS_DIR ${SRC_DIR}/utils)
set(STRUCTURES_DIR ${SRC_DIR}/structures)

# List of source files
set(MSDF_SRC
    ${MSDF_DIR}/core/shape/color.cpp
    ${MSDF_DIR}/core/shape/segment.cpp
    ${MSDF_DIR}/core/shape/linear_segment.cpp
    ${MSDF_DIR}/core/shape/quadratic_segment.cpp
    ${MSDF_DIR}/core/shape/cubic_segment.cpp
    ${MSDF_DIR}/core/shape/edge.cpp
    ${MSDF_DIR}/core/shape/shape.cpp
    ${MSDF_DIR}/core/shape/contour.cpp
    ${MSDF_DIR}/core/channel.cpp
    ${MSDF_DIR}/core/contour_combiners.cpp
    ${MSDF_DIR}/core/distance_mapping.cpp
    ${MSDF_DIR}/core/distance_selector.cpp
    ${MSDF_DIR}/core/edge_cache.cpp
    ${MSDF_DIR}/core/edge_coloring.cpp
    ${MSDF_DIR}/core/equation_solver.cpp
    ${MSDF_DIR}/core/error_correction.cpp
    ${MSDF_DIR}/core/font.cpp
    ${MSDF_DIR}/core/generate_msdf.cpp
    ${MSDF_DIR}/core/glyph.cpp
    ${MSDF_DIR}/core/projection.cpp
    ${MSDF_DIR}/core/rasterization.cpp
    ${MSDF_DIR}/core/scanline.cpp
    ${MSDF_DIR}/core/shape_distance_finder.cpp
    ${MSDF_DIR}/atlas/blit.cpp
    ${MSDF_DIR}/atlas/charset.cpp
    ${MSDF_DIR}/atlas/font_geometry.cpp
    ${MSDF_DIR}/atlas/generate_atlas.cpp
    ${MSDF_DIR}/atlas/glyph_generator.cpp
    ${MSDF_DIR}/atlas/glyph_geometry.cpp
    ${MSDF_DIR}/atlas/grid_atlas_packer.cpp
    ${MSDF_DIR}/atlas/padding.cpp
    ${MSDF_DIR}/atlas/rectangle_packer.cpp
    ${MSDF_DIR}/atlas/size_selectors.cpp
    ${MSDF_DIR}/atlas/tight_atlas_packer.cpp
    ${MSDF_DIR}/atlas/utf8.cpp
    ${MSDF_DIR}/atlas/workload.cpp
    ${MSDF_DIR}/msdf_atlas.cpp
    ${MSDF_DIR}/msdf.cpp
)
set(INFO_SRC
    ${INFO_DIR}/endianness.cpp
)
set(UTILS_SRC
    ${UTILS_DIR}/timer.cpp
    ${UTILS_DIR}/reader.cpp
)
set(STRUCTURES_SRC
    ${STRUCTURES_DIR}/truetypefont/truetypefont.cpp
)


# List of all source files
set(SOURCES
    ${MSDF_SRC}
    ${INFO_SRC}
    ${UTILS_SRC}
    ${STRUCTURES_SRC}
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

    ${MSDF_DIR}
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