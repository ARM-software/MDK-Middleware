# CMakeLists.txt for calling FCARM
# Find input files in the input base directory and in its subfolders using recursive scanning
# Format arguments and generate a steering command file, overcoming any command line length limitation
# Call FCARM using the steering command file, generating the source file in the expected output  
#
# Configuration Step: ${CMAKE_COMMAND} -G <generator> -S <source directory> -B <build directory> -DINPUT_DIRECTORY=<input base directory> -DOUTPUT=<output source file>
# Build Step: ${CMAKE_COMMAND} --build <build directory>
#
# <generator>: underlying generator build system, e.g. Ninja
# <source directory>: directory where this CMakeLists.txt resides
# <build directory>: directory for temp files
# <input base directory>: directory where input data is located
# <output source file>: path and filename of source file to be generated

cmake_minimum_required(VERSION 3.22)
include(ExternalProject)

project("FCARM" NONE)

file(GLOB_RECURSE INPUT ${INPUT_DIRECTORY}/*)

foreach(ITEM ${INPUT})
  cmake_path(RELATIVE_PATH ITEM BASE_DIRECTORY ${INPUT_DIRECTORY} OUTPUT_VARIABLE FILE)
  list(APPEND FILES ${FILE})
endforeach()
string(REPLACE ";" ",\n" FILES "${FILES}")

cmake_path(RELATIVE_PATH OUTPUT BASE_DIRECTORY ${INPUT_DIRECTORY} OUTPUT_VARIABLE RELATIVE_OUTPUT)
cmake_path(GET INPUT_DIRECTORY FILENAME INPUT_DIRECTORY_NAME)
cmake_path(GET INPUT_DIRECTORY PARENT_PATH WORKING_DIRECTORY)

set(COMMAND_FILE "${CMAKE_CURRENT_BINARY_DIR}/Auto_FcArm_Cmd.inp")
file(WRITE ${COMMAND_FILE} "${FILES}\nTO ${RELATIVE_OUTPUT} RTE NOPRINT ROOT(${INPUT_DIRECTORY_NAME})\n")

add_custom_target(FCARM ALL DEPENDS ${OUTPUT})
add_custom_command(OUTPUT ${OUTPUT} DEPENDS ${INPUT} ${INPUT_DIRECTORY}
  COMMAND fcarm @${COMMAND_FILE}
  WORKING_DIRECTORY ${WORKING_DIRECTORY}
)
