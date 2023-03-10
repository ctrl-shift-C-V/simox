PROJECT ( ColladaViewer )

IF(Simox_USE_COIN_VISUALIZATION AND Simox_USE_COLLADA)


    GET_FILENAME_COMPONENT (CurrentPath ${CMAKE_CURRENT_LIST_FILE} PATH)

    SET(DEMO_BASE_DIR ${PROJECT_SOURCE_DIR})

	FILE(GLOB demo_SRCS ${PROJECT_SOURCE_DIR}/RobotViewer.cpp
	                    ${PROJECT_SOURCE_DIR}/showRobotWindow.cpp
	                    ${PROJECT_SOURCE_DIR}/COLLADA/ColladaIO.cpp
	                    ${PROJECT_SOURCE_DIR}/../COLLADA/ColladaParser.cpp)

	FILE(GLOB demo_INCS ${PROJECT_SOURCE_DIR}/showRobotWindow.h
	                    ${PROJECT_SOURCE_DIR}/COLLADA/ColladaIO.h
	                    ${PROJECT_SOURCE_DIR}/../COLLADA/ColladaParser.h
	                    ${PROJECT_SOURCE_DIR}/../COLLADA/ColladaIterators.h)



    ################################## moc'ing ##############################
    set(mocFiles
        ${PROJECT_SOURCE_DIR}/showRobotWindow.h
    )

    set(uiFiles
        ${PROJECT_SOURCE_DIR}/RobotViewer.ui
    )

	# create the executable
  VirtualRobotQtApplication(${PROJECT_NAME} "${demo_SRCS}" "${demo_INCS}" "${mocFiles}" "${uiFiles}")
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${Simox_BIN_DIR})
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES FOLDER "Examples")

  TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC
      -DDEMO_BASE_DIR="${DEMO_BASE_DIR}"
      -DDEMO_PATH="${PROJECT_SOURCE_DIR}"
      -DDOM_DYNAMIC
    )
  #######################################################################################
  ############################ Setup for installation ###################################
  #######################################################################################

  install(TARGETS ${PROJECT_NAME}
    # IMPORTANT: Add the library to the "export-set"
    EXPORT SimoxTargets
    RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
    COMPONENT dev)

  MESSAGE( STATUS " ** Simox application ${PROJECT_NAME} will be placed into " ${Simox_BIN_DIR})
  MESSAGE( STATUS " ** Simox application ${PROJECT_NAME} will be installed into " ${INSTALL_BIN_DIR})

ENDIF()
