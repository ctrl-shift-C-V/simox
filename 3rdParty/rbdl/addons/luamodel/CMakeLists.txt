CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake )

SET_TARGET_PROPERTIES ( ${PROJECT_EXECUTABLES} PROPERTIES
  LINKER_LANGUAGE CXX
  )

FIND_PACKAGE (Lua51 REQUIRED)

INCLUDE_DIRECTORIES (
  ${CMAKE_CURRENT_BINARY_DIR}/include/rbdl
  ${LUA_INCLUDE_DIR}
  )

SET ( LUAMODEL_SOURCES
  luamodel.cc
  luatables.cc
  )

ADD_EXECUTABLE (rbdl_luamodel_util rbdl_luamodel_util.cc)

IF (RBDL_BUILD_STATIC)
  ADD_LIBRARY ( rbdl_luamodel-static STATIC ${LUAMODEL_SOURCES} )
  IF (NOT WIN32)
    SET_TARGET_PROPERTIES ( rbdl_luamodel-static PROPERTIES PREFIX "lib")
  ENDIF (NOT WIN32)
  SET_TARGET_PROPERTIES ( rbdl_luamodel-static PROPERTIES OUTPUT_NAME "rbdl_luamodel")

  TARGET_LINK_LIBRARIES (rbdl_luamodel-static
    rbdl-static
    ${LUA_LIBRARIES}
    )

  TARGET_LINK_LIBRARIES (rbdl_luamodel_util
    rbdl_luamodel-static
    rbdl-static
    )

  INSTALL (TARGETS rbdl_luamodel-static rbdl_luamodel_util
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
ELSE (RBDL_BUILD_STATIC)
  ADD_LIBRARY ( rbdl_luamodel SHARED ${LUAMODEL_SOURCES} )
  SET_TARGET_PROPERTIES ( rbdl_luamodel PROPERTIES
    VERSION ${RBDL_VERSION}
    SOVERSION ${RBDL_SO_VERSION}
    )

  TARGET_LINK_LIBRARIES (rbdl_luamodel
    rbdl
    ${LUA_LIBRARIES}
    )

  TARGET_LINK_LIBRARIES (rbdl_luamodel_util
    rbdl_luamodel
    rbdl
    )

  INSTALL (TARGETS rbdl_luamodel rbdl_luamodel_util
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
ENDIF (RBDL_BUILD_STATIC)

FILE ( GLOB headers
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  )

INSTALL ( FILES ${headers}
  DESTINATION
  ${CMAKE_INSTALL_INCLUDEDIR}/rbdl/addons/luamodel
  )
