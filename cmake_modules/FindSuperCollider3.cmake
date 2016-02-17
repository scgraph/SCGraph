# Find the SuperCollider development headers
#
# This module defines these variables:
#
#  SC_FOUND
#   True if the SC3 header files were found
#
#  SC_PATH
#   The path to the SC source+header files


#
# Find the telltale header file
#
GET_FILENAME_COMPONENT(SOURCEPARENT "${CMAKE_CURRENT_SOURCE_DIR}" PATH)
find_path(
  SC3_SOURCES_PATH NAMES include/plugin_interface/SC_PlugIn.h
  HINTS "~/src/supercollider" "~/dev/supercollider"
  PATHS "${SOURCEPARENT}"
  PATH_SUFFIXES SuperCollider
  DOC "Path to the SuperCollider source and header files.")

set(SC3_SOURCES_FOUND FALSE)

if(IS_DIRECTORY ${SC3_SOURCES_PATH})
  set(SC3_SOURCES_FOUND TRUE)
endif()
