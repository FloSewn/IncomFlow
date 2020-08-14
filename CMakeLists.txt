##############################################################
#                      LEARN C THE HARD WAY                  #
##############################################################
# prerequisites
cmake_minimum_required( VERSION 3.5.2 )
project( Incompressible-Flow-Simulator )

# enable testing support
enable_testing()

# Include cmake files
include( "${CMAKE_SOURCE_DIR}/share/cmake/directories.cmake" )
include( "${CMAKE_SOURCE_DIR}/share/cmake/compiler_flags.cmake" )

#-------------------------------------------------------------
# Define modules
#-------------------------------------------------------------
add_subdirectory( ${SRC}/incomflow )
