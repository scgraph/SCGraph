################################################################################

find_package(Qt4 REQUIRED QtCore QtGui QtOpenGL)

find_package(OpenGL)

find_package(PkgConfig)

find_package(Threads)

find_package(Boost)

################################################################################

set(scgraph_srcs main cow_ptr transformation_command
  transformation_command_visitor graphics_visitor obj_loader loader_3d
  buffer_pool matrix node_tree synth node group options scgraph synthdef
  control_loop graphic_loop synthdef_file util synthdef_pool plugin_pool
  plugin unit  osc_handler graphics tree in_out control graphics_bus 
  material face color_rgba 
  vector_3d hvector_3d ofVec2f 
  sc_unit_wrapper sc__unit texture
  texture_pool shader_pool)


# configure a header file to pass some of the CMake settings
# to the source code
# configure_file (
#   "${PROJECT_SOURCE_DIR}/src/main.h.in"
#   "${PROJECT_BINARY_DIR}/src/main.h"
#   )
################################################################################
# set flag for SuperCollider 3.6

if(SC3_VERSION_36)
  add_definitions(-DSC36)
endif(SC3_VERSION_36)

################################################################################
# add glew

include_directories(glew)

list(APPEND scgraph_srcs glew/glew)

add_definitions(-DGLEW_MX)

################################################################################
# add oscpack

include_directories(${CMAKE_BINARY_DIR} oscpack)

set(oscpack_srcs 
  oscpack/osc/OscOutboundPacketStream 
  oscpack/osc/OscPrintReceivedElements 
  oscpack/osc/OscReceivedElements 
  oscpack/osc/OscTypes 
  oscpack/ip/IpEndpointName )

if(LINUX OR APPLE)
  list(APPEND oscpack_srcs
    oscpack/ip/posix/NetworkingUtils 
    oscpack/ip/posix/UdpSocket)
else()
  list(APPEND oscpack_srcs
    oscpack/ip/win32/NetworkingUtils 
    oscpack/ip/win32/UdpSocket)
endif()

add_definitions(-DOSC_HOST_LITTLE_ENDIAN)
add_library(oscpack STATIC ${oscpack_srcs})

################################################################################
# add jack if wanted

if(JACK)

  find_package(FFTW3f)

  find_package(Jack)

  if (!JACK_FOUND)
    message(SEND_ERROR "Cannot find jack!")
  endif()

  if (!FFTW3F_FOUND)
    message(SEND_ERROR "Cannot find libfftw3f!")
  endif()

  list(APPEND scgraph_srcs jack_client)

  include_directories(${FFTW3F_INCLUDE_DIR})
  include_directories(${JACK_INCLUDE_DIRS})

  add_definitions(-DHAVE_JACK)
endif()

################################################################################
# text support

if(TEXT)
  find_package(FTGL)

  find_package(Freetype)

  if(!FTGL_FOUND)
	message(SEND_ERROR "FTGL not found. Text support disabled.")
  elseif(!FREETYPE_FOUND)
	message(SEND_ERROR "Freetype not found. Text support disabled.")
  endif()

  message(STATUS "Compiling with text support.")

  include_directories(${FREETYPE_INCLUDE_DIRS})
  include_directories(${FTGL_INCLUDE_DIR})

  # # work around bug in current homebrew version of ftgl
  # if(APPLE) 
  # 	  include_directories(${FREETYPE_INCLUDE_DIRS}/freetype2)
  # endif()

  add_definitions(-DHAVE_FTGL)

  list(APPEND scgraph_moc_hdrs string_pool.h)

  list(APPEND scgraph_srcs string_pool)
endif()

################################################################################
# video support

if(VIDEO)
  find_package(FFmpeg REQUIRED SWSCALE AVFORMAT AVCODEC AVUTIL)

  if (NOT FFMPEG_FOUND)
	message(SEND_ERROR "ffmpeg not found. video loading disabled")
  endif()

  message(STATUS "Compiling with video support.")

  include_directories(${FFMPEG_INCLUDE_DIR})
  add_definitions(-DHAVE_FFMPEG)

  list(APPEND scgraph_srcs video_texture)

  list(APPEND scgraph_moc_hdrs video_texture.h)
endif()

################################################################################
# qt-specific stuff

list(APPEND scgraph_moc_hdrs
  graphics.h graphic_loop.h osc_handler.h
  texture_pool.h shader_pool.h)

# add some useful macros and variables
INCLUDE( ${QT_USE_FILE} )

# this will run moc:
QT4_WRAP_CPP( scgraph_moc_srcs ${scgraph_moc_hdrs} )
qt4_add_resources( scgraph_rcc resources.qrc )

################################################################################
# sc includes

include_directories(${SC3_SOURCES_PATH}/common)
include_directories(${SC3_SOURCES_PATH}/plugin_interface)

include_directories(${SC3_SOURCES_PATH}/include/plugin_interface)
include_directories(${SC3_SOURCES_PATH}/include/common)

include_directories(${SC3_SOURCES_PATH}/server/plugins) # for FFT_UGens.h

include_directories(${SC3_SOURCES_PATH}/external_libraries/libsndfile)

if(SC3_VERSION_36)
  # for SC_Group.h
  include_directories(${SC3_SOURCES_PATH}/include/server)
endif()

add_definitions(
  -DSC3_PLUGIN_PATH="${SC3_PLUGIN_PATH}"
  -DPREFIX="${CMAKE_INSTALL_PREFIX}")

if(APPLE)
  add_definitions(-DSC_DARWIN)
endif(APPLE)

################################################################################
### boost

include_directories(${Boost_INCLUDE_DIRS})

################################################################################
### build and link

add_executable( scgraph ${scgraph_srcs} 
  ${scgraph_moc_srcs}
  ${scgraph_rcc} )

target_link_libraries(scgraph 
  ${QT_LIBRARIES} 
  ${CMAKE_THREAD_LIBS_INIT}
  -L${SC3_LIBRARY_PATH}
  ${OPENGL_LIBRARIES}
  ${FREETYPE_LIBRARIES}
  ${FTGL_LIBRARIES}
  ${FFMPEG_LIBRARIES}
  ${JACK_LIBRARIES}
  ${FFTW3F_LIBRARY}
  scsynth
  stdc++
  m
  oscpack)

if(APPLE)
  target_link_libraries(scgraph
	-Wl,-bind_at_load)
elseif(LINUX)
  TARGET_LINK_LIBRARIES( scgraph 
    -Wl,--export-dynamic 
    GLU 
    -rdynamic 
    dl)
else(APPLE)
  TARGET_LINK_LIBRARIES( scgraph 
    -Wl,--export-dynamic 
    GLU 
    -rdynamic 
    dl)
endif(APPLE)

################################################################################
### install

install(PROGRAMS ${CMAKE_BINARY_DIR}/src/scgraph DESTINATION "bin/")

################################################################################

add_subdirectory(plugins)

add_subdirectory(tests)
