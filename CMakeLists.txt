project(codal-atmega328p)
enable_language(ASM)

include("${CODAL_UTILS_LOCATION}")

# find sources and headers
RECURSIVE_FIND_DIR(TOP_LEVEL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/inc" "*.h")
RECURSIVE_FIND_FILE(TOP_LEVEL_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/source" "*.c??")

# add them
include_directories(${TOP_LEVEL_INCLUDE_DIRS})

# create our target
add_library(codal-atmega328p ${TOP_LEVEL_SOURCE_FILES})

target_link_libraries(
    codal-atmega328p
    codal-core
)

# expose it to parent cmake.
target_include_directories(codal-atmega328p PUBLIC ${TOP_LEVEL_INCLUDE_DIRS})
