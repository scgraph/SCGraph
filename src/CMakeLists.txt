################################################################################

find_package(Qt4 REQUIRED QtCore QtGui QtOpenGL)

find_package(OpenGL)

find_package(PkgConfig)

find_package(Threads)

################################################################################

pkg_check_modules (GraphicsMagick++ GraphicsMagick++)

if(!PKGCONFIG_FOUND)
  find_package(GraphicsMagick++)
  if(!GraphicsMagick++_FOUND)
    message(SEND_ERROR "Cannot find graphicsmagic")
  endif(!GraphicsMagick++_FOUND)
endif(!PKGCONFIG_FOUND)

include_directories(${GraphicsMagick++_INCLUDE_DIRS})

################################################################################

set(scgraph_srcs main cow_ptr transformation_command transformation_command_visitor graphics_visitor obj_loader loader_3d buffer_pool matrix node_tree synth node group options scgraph synthdef control_loop graphic_loop synthdef_file util synthdef_pool plugin_pool plugin unit  osc_handler graphics tree in_out control graphics_bus material face color_rgba vector_3d hvector_3d vector_2d sc_unit_wrapper sc__unit texture_pool ui_pool shader_pool)

# configure a header file to pass some of the CMake settings
# to the source code
# configure_file (
#   "${PROJECT_SOURCE_DIR}/src/main.h.in"
#   "${PROJECT_BINARY_DIR}/src/main.h"
#   )

################################################################################
# add glew

include_directories(glew)

set(scgraph_srcs ${scgraph_srcs} glew/glew)

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

if(POSIX OR APPLE)
  set(oscpack_srcs ${oscpack_srcs}
    oscpack/ip/posix/NetworkingUtils 
    oscpack/ip/posix/UdpSocket)
else()
  set(oscpack_srcs ${oscpack_srcs}
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
    message(SEND_ERROR "Cannot find libfftw3f")
  endif()

  set(scgraph_srcs ${scgraph_srcs} jack_client)
endif()

################################################################################
# qt-specific stuff

set(scgraph_moc_hdrs graphic_loop.h osc_handler.h texture_pool.h shader_pool.h)

# add some useful macros and variables
INCLUDE( ${QT_USE_FILE} )

# this will run moc:
QT4_WRAP_CPP( scgraph_moc_srcs ${scgraph_moc_hdrs} )

################################################################################
# sc includes

include_directories(${SC_PATH}/common)
include_directories(${SC_PATH}/plugin_interface)

include_directories(${SC_PATH}/include/server) # for SC_Group.h
include_directories(${SC_PATH}/include/plugin_interface)
include_directories(${SC_PATH}/include/common)

include_directories(${SC_PATH}/server/plugins) # for FFT_UGens.h

include_directories(${SC_PATH}/external_libraries/libsndfile)

################################################################################

add_definitions(
  -DSC3_PLUGIN_PATH="${SC_PATH}/build/SuperCollider/plugins"
  -DPREFIX="${CMAKE_INSTALL_PREFIX}")

if(APPLE)
  add_definitions(-DSC_DARWIN)
endif(APPLE)


################################################################################
### build and link

add_executable( scgraph ${scgraph_srcs} ${scgraph_moc_srcs} )

if(APPLE)
  TARGET_LINK_LIBRARIES( scgraph 
    ${QT_LIBRARIES} 
    ${CMAKE_THREAD_LIBS_INIT}
    ${GraphicsMagick++_LDFLAGS} 
    ${GraphicsMagick++_CFLAGS}
    -L${SC3_LIBRARY_PATH}
    ${OPENGL_LIBRARIES}
    scsynth
    stdc++
    m
    oscpack
    -Wl,-bind_at_load
    )
elseif(LINUX)
TARGET_LINK_LIBRARIES( scgraph 
    ${CMAKE_THREAD_LIBS_INIT}
    ${GraphicsMagick++_LDFLAGS} 
    ${GraphicsMagick++_CFLAGS}
    -L${SC3_LIBRARY_PATH}
    scsynth
    stdc++
    ${QT_LIBRARIES} 
    m
    oscpack
    -Wl,--export-dynamic 
    GLU 
    -rdynamic 
    dl
    )
else(APPLE)
TARGET_LINK_LIBRARIES( scgraph 
    ${CMAKE_THREAD_LIBS_INIT}
    ${GraphicsMagick++_LDFLAGS} 
    ${GraphicsMagick++_CFLAGS}
    -L${SC3_LIBRARY_PATH}
    scsynth
    stdc++
    ${QT_LIBRARIES} 
    m
    oscpack
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