################################################################################

cmake_minimum_required (VERSION 2.8)
project (scgraph)

# The version number.
set (scgraph_VERSION_MAJOR 0)
set (scgraph_VERSION_MINOR 14)

################################################################################

if (APPLE OR WIN32)
  set(CMAKE_SHARED_MODULE_SUFFIX ".bundle")
  set(PLUGIN_EXTENSION ".bundle")
endif()

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
        set(LINUX 1)
endif()
################################################################################

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules
                      ${CMAKE_MODULE_PATH})

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

add_subdirectory(src)

################################################################################

add_custom_target(uninstall
3  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

################################################################################

find_package(SuperCollider3)
if (NOT SC3_SOURCES_FOUND)
	message(SEND_ERROR "Cannot find SuperCollider3 headers. Please set the variable SC3_SOURCES_PATH.")
else()
	message(STATUS "Using SC source located at ${SC3_SOURCES_PATH}")
endif()

################################################################################

# this doesn't seem to be used anyways
#option(GLEW "If you have libGLEW installed and want scgraph to check for shader extensions before using them" ON)

option(SHADERS "Build with experimental vertex and fragment shader support" ON)
option(JACK "Build with jack support for the FFT UGen")

# option(QUARKS "Install plugins as quarks")
# option(OSX_PACKAGE "Package dmg for Apple")
# option(IN_PLACE_BUILD "Build and install in cmake build folder" ON)



################################################################################
option(VIDEO "Support video loading (needs ffmpeg)" ON)

if(VIDEO)
  find_package(FFmpeg REQUIRED SWSCALE AVFORMAT AVCODEC AVUTIL)
  if (NOT FFMPEG_FOUND)
	message(STATUS "ffmpeg not found. video loading disabled")
	set(VIDEO, false)
  endif()
endif(VIDEO)

# # osx `make install' defaults into cmake_build/SC3-plugins directory
# if (APPLE AND IN_PLACE_BUILD)
#   set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}")
# endif()

# # when packaging OSX dmg, install in 'cmake_build/build_osx' so we can include
# # directly the created SC3plugins subfolder in the root of the dmg (instead of
# # all plugins directories in the dmg root)
# if (APPLE AND OSX_PACKAGE)
#   set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/build_osx")
# endif()

# if (QUARKS OR OSX_PACKAGE) # add quarks DIRECTORY in osx dmg
#   add_subdirectory(quarks)
# endif()

# if (OSX_PACKAGE)
#   add_subdirectory(osx_package)
# endif()


if (LINUX)
   find_path(SC3_LIBRARY_PATH "libscsynth.so" 
	 HINTS "/usr/lib"
	 "/usr/local/lib" DOC "Path where libscsynth is found")
   find_path(SC3_PLUGIN_PATH "MulAddUGens.so"
	 HINTS "/usr/lib/SuperCollider/plugins"
	 "/usr/local/SuperCollider/plugins" DOC "Path where the UGens are found")
elseif(APPLE)
   find_path(SC3_LIBRARY_PATH "libscsynth.dylib" 
	 HINTS "/usr/lib" "/usr/local/lib"
	 "/Applications/SuperCollider/SuperCollider.app/Contents/Resources"
	 DOC "Path where libscsynth is found")
   find_path(SC3_PLUGIN_PATH "MulAddUGens.scx"
	 HINTS "/usr/lib" "/usr/local/lib"
	 "/Applications/SuperCollider/SuperCollider.app/Contents/Resources/plugins"
	 DOC "Path where the UGens are found")
endif(LINUX)


set(plugins "")

set(INSTALL_DESTINATION_PLUGINS "lib/scgraph/plugins")

if (LINUX)
    set(INSTALL_DESTINATION "lib/SuperCollider/plugins/scgraph")
    if (QUARKS)
        set(INSTALL_DESTINATION_DISTRO
                "share/SuperCollider/scgraph")
    else()
        set(INSTALL_DESTINATION_DISTRO
                "share/SuperCollider/Extensions/scgraph")
    endif(QUARKS)
elseif(APPLE)
    set(INSTALL_DESTINATION "scgraph")
    set(INSTALL_DESTINATION_DISTRO "SCGraph")
endif(LINUX)

################################################################################
### install docs, sc classes and examples

if(APPLE)
  if(JACK)
    install(DIRECTORY src/sc/ DESTINATION
      ${CMAKE_BINARY_DIR}/${INSTALL_DESTINATION_DISTRO})
  else(JACK)
    install(DIRECTORY src/sc/ DESTINATION
      ${CMAKE_BINARY_DIR}/${INSTALL_DESTINATION_DISTRO}
      PATTERN "GFreqAmp*" EXCLUDE)
  endif(JACK)
else(APPLE)
  if(JACK)
    install(DIRECTORY src/sc/ DESTINATION ${INSTALL_DESTINATION_DISTRO})
  else(JACK)
    install(DIRECTORY src/sc/ DESTINATION ${INSTALL_DESTINATION_DISTRO} 
      PATTERN "GFreqAmp*" EXCLUDE)
  endif(JACK)
endif(APPLE)

install(DIRECTORY doc/examples DESTINATION share/scgraph)
