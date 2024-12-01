cmake_minimum_required(VERSION 3.5.0)
project(Qontext VERSION 0.1.0 LANGUAGES C CXX)

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
set(CORE ${SRC_DIR}/core)
set(FILE_TYPES ${SRC_DIR}/file_types)
set(GENERATOR ${SRC_DIR}/generator)
set(READER ${SRC_DIR}/reader)
set(WRITER ${SRC_DIR}/writer)

# List of source files
set(CORE_SOURCES
    ${CORE}/info/endianness.cpp
)
set(FILE_TYPES_SOURCES

)
set(GENERATOR_SOURCES
    ${GENERATOR}/msdf/core/projection.cpp
    ${GENERATOR}/msdf/core/scanline.cpp
    ${GENERATOR}/msdf/core/distance_mapping.cpp
    ${GENERATOR}/msdf/core/equation_solver.cpp
    ${GENERATOR}/msdf/core/edge_segments.cpp
    ${GENERATOR}/msdf/core/edge_holder.cpp
    ${GENERATOR}/msdf/core/edge_selectors.cpp
    ${GENERATOR}/msdf/core/contour.cpp
    ${GENERATOR}/msdf/core/shape.cpp
    ${GENERATOR}/msdf/core/glyph.cpp
    ${GENERATOR}/msdf/core/contour_combiners.cpp
    ${GENERATOR}/msdf/core/edge_coloring.cpp
    ${GENERATOR}/msdf/core/msdf_error_correction.cpp
    ${GENERATOR}/msdf/core/render_sdf.cpp
    ${GENERATOR}/msdf/core/rasterization.cpp
    ${GENERATOR}/msdf/core/sdf_error_correction.cpp
    ${GENERATOR}/msdf/core/resolve_shape_geometry.cpp
    ${GENERATOR}/msdf/core/msdf_font.cpp
    ${GENERATOR}/msdf/msdf.cpp
    ${GENERATOR}/msdf/atlas/utf8.cpp
    ${GENERATOR}/msdf/atlas/padding.cpp
    ${GENERATOR}/msdf/atlas/charset.cpp
    ${GENERATOR}/msdf/atlas/glyph_geometry.cpp
    ${GENERATOR}/msdf/atlas/font_geometry.cpp
    ${GENERATOR}/msdf/atlas/rectangle_packer.cpp
    ${GENERATOR}/msdf/atlas/workload.cpp
    ${GENERATOR}/msdf/atlas/size_selectors.cpp
    ${GENERATOR}/msdf/atlas/image_blit.cpp
    ${GENERATOR}/msdf/atlas/tight_atlas_packer.cpp
    ${GENERATOR}/msdf/atlas/grid_atlas_packer.cpp
    ${GENERATOR}/msdf/atlas/glyph_generators.cpp
    ${GENERATOR}/msdf/msdf_atlas.cpp
)
set(READER_SOURCES
    ${READER}/reader.cpp
    ${READER}/ttf_reader.cpp
)
set(WRITER_SOURCES

)

# List of all source files
set(SOURCES
    ${CORE_SOURCES}
    ${READER_SOURCES}
    ${GENERATOR_SOURCES}
)

# Add the Qontext library
add_library(Qontext STATIC ${SOURCES})

# *********************************************************************************************
#                                  INCLUDE LOCATION DEFINITIONS
# *********************************************************************************************

set(CORE_DIR ${INCLUDE_DIR}/core)
set(FILETYPES_DIR ${INCLUDE_DIR}/file_types)
set(GENERATOR_DIR ${INCLUDE_DIR}/generator)
set(READER_DIR ${INCLUDE_DIR}/reader)
set(WRITER_DIR ${INCLUDE_DIR}/writer)

# Include directories for the Qontext library
target_include_directories(Qontext PRIVATE
    ${INCLUDE_DIR}
    
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
    
    ${CORE_DIR}
    ${FILETYPES_DIR}
    ${GENERATOR_DIR}
    ${READER_DIR}
    ${WRITER_DIR}
    
    # external
    ${BASE_DIR}/external/qarbon/include
)

# Link the Qontext library to the QontextDebug executable
target_link_libraries(QontextDebug Qontext)