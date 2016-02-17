################################################################################
### shaders

if(SHADERS)
  add_definitions(-DHAVE_SHADERS)
endif()

################################################################################
### sources

set(plugins_srcs 
  buf_info_ugens frame_rate render_mode shader_program
  gl_renderer model 
  translate rotate stretch transformation
  binary_op sum3 sum4 mul_add 
  color material light blending lighting culling
  line points triangle circle cube tube 
  bordered_rectangle rectangle polygon quad)

################################################################################
### qt
# add some useful macros and variables
INCLUDE( ${QT_USE_FILE} )

################################################################################
### text

if (TEXT)
  list(APPEND plugins_srcs text)
endif()


################################################################################
### want jack?

if (JACK)
  list(APPEND plugins_srcs freq_amp)
endif()


################################################################################
### includes

include_directories(${CMAKE_BINARY_DIR})

include_directories(${SC3_SOURCES_PATH}/include/plugin_interface)
include_directories(${SC3_SOURCES_PATH}/include/common)
include_directories(${SC3_SOURCES_PATH}/include/server) # for SC_Group.h

include_directories(${SC3_SOURCES_PATH}/external_libraries/libsndfile)

include_directories(../glew)
include_directories(../oscpack)

################################################################################

add_definitions(-DGLEW_MX)

add_definitions(-DPREFIX="${CMAKE_INSTALL_PREFIX}")

set(CMAKE_SHARED_MODULE_PREFIX "")
set(CMAKE_SHARED_MODULE_SUFFIX ".so")

if(APPLE)
  add_definitions(-DSC_DARWIN)
  set(CMAKE_SHARED_MODULE_SUFFIX ".bundle")
endif(APPLE)

message(STATUS ${JACK_LIBRARIES})

################################################################################
### link qt, build, link and install

link_directories(${QT_LIBRARIES})

foreach(plugin ${plugins_srcs})
  QT4_WRAP_CPP(${plugin}_moc_src ${plugin}.h)
  add_library(${plugin} MODULE ${plugin} ${${plugin}_moc_src})
  if(APPLE)
    target_link_libraries(${plugin}
	  -bundle 
	  -fPIC 
	  "-undefined dynamic_lookup")
  elseif(LINUX)
    target_link_libraries(${plugin} 
	  -shared 
	  -fpic 
	  -export-dynamic 
	  -rdynamic 
	  dl
	  "")
  else(APPLE)
    target_link_libraries(${plugin} 
	  -shared 
	  -fpic 
	  -export-dynamic 
	  -rdynamic 
	  dl
	  "")
  endif(APPLE)

  install(FILES
	${CMAKE_BINARY_DIR}/src/plugins/${plugin}${CMAKE_SHARED_MODULE_SUFFIX}
    DESTINATION "lib/scgraph/plugins/")
endforeach(plugin)
