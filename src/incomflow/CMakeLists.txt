##############################################################
# MODULE: LEARN C THE HARD WAY
##############################################################
# Define directories
set( INCOMFLOW_DIR    ${SRC}/incomflow         )
set( INCOMFLOW_INC    ${INCOMFLOW_DIR}/include )
set( INCOMFLOW_SRC    ${INCOMFLOW_DIR}/src     )

# Define source files
set( INCOMFLOW_MAIN
  ${INCOMFLOW_SRC}/icfList.c
  ${INCOMFLOW_SRC}/icfNode.c
  ${INCOMFLOW_SRC}/icfEdge.c
  ${INCOMFLOW_SRC}/icfTri.c
  ${INCOMFLOW_SRC}/icfMesh.c
  ${INCOMFLOW_SRC}/icfFlowData.c
  )

##############################################################
# LIBRARY: incomflow
##############################################################
# define name
set( INCOMFLOW_LIB incomflow )

# add sources to library
add_library( ${INCOMFLOW_LIB} STATIC ${INCOMFLOW_MAIN} )
#add_library( ${INCOMFLOW_LIB} SHARED ${INCOMFLOW_MAIN} )

# set public include
target_include_directories( ${INCOMFLOW_LIB} PUBLIC
  $<BUILD_INTERFACE:${INCOMFLOW_INC}>
  PRIVATE ${INCOMFLOW_SRC}
)

target_link_libraries( ${INCOMFLOW_LIB}
  m
)

install( TARGETS incomflow DESTINATION ${LIB} )

##############################################################
# MAIN: incomflow
##############################################################
set( MAINEXE_INCOMFLOW IncomFlow )

add_executable( ${MAINEXE_INCOMFLOW}
  ${INCOMFLOW_SRC}/main.c
)

target_include_directories( ${MAINEXE_INCOMFLOW} PUBLIC
  $<BUILD_INTERFACE:${INC_INCOMFLOW}>
)

target_link_libraries( ${MAINEXE_INCOMFLOW}
  incomflow
  m
)

# Install executables
install( TARGETS ${MAINEXE_INCOMFLOW} RUNTIME DESTINATION ${BIN} )


##############################################################
# TESTS: incomflow
##############################################################
set( TESTEXE_INCOMFLOW incomflow_test        )
set( TESTDIR_INCOMFLOW ${INCOMFLOW_DIR}/test )

add_executable( ${TESTEXE_INCOMFLOW}
  ${TESTDIR_INCOMFLOW}/basic_tests.c
  ${TESTDIR_INCOMFLOW}/unit_tests.c
)

target_include_directories( ${TESTEXE_INCOMFLOW} PUBLIC
  $<BUILD_INTERFACE:${INC_INCOMFLOW}>
)

target_link_libraries( ${TESTEXE_INCOMFLOW}
  incomflow
  m
)

# Install executables
install( TARGETS ${TESTEXE_INCOMFLOW} RUNTIME DESTINATION ${BIN} )



