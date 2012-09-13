################################################################################
# build tests
################################################################################

include_directories(../)

add_definitions(-Wall -pedantic)

set(scgraph_test_moc_hdrs ../graphics.h)

# add some useful macros and variables
INCLUDE( ${QT_USE_FILE} )

# this will run moc:
QT4_WRAP_CPP( scgraph_test_moc_srcs ${scgraph_test_moc_hdrs} )

add_executable(test_tree test_tree ../tree)

add_executable(test_obj_loader test_obj_loader ${scgraph_test_moc_srcs} ../obj_loader ../loader_3d
../graphics ../matrix ../vector_3d ../face ../color_rgba ../material)

if(APPLE)
  TARGET_LINK_LIBRARIES(test_obj_loader 
    ${QT_LIBRARIES} 
    -Wl,-bind_at_load
    )
elseif(LINUX)
TARGET_LINK_LIBRARIES( test_obj_loader
    ${QT_LIBRARIES} 
    -Wl,--export-dynamic 
    -rdynamic 
    )
else(APPLE)
TARGET_LINK_LIBRARIES( test_obj_loader
    ${QT_LIBRARIES} )
endif(APPLE)

