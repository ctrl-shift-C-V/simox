PROJECT ( MultiThreadedPlanning )

IF(Simox_VISUALIZATION AND Simox_USE_COIN_VISUALIZATION)
	
	# the variable "demo_SRCS" contains all .cpp files of this project
	FILE(GLOB demo_SRCS ${PROJECT_SOURCE_DIR}/MTPlanning.cpp ${PROJECT_SOURCE_DIR}/MTPlanningWindow.cpp ${PROJECT_SOURCE_DIR}/MTPlanningScenery.cpp)
	FILE(GLOB demo_INCS ${PROJECT_SOURCE_DIR}/MTPlanningWindow.h ${PROJECT_SOURCE_DIR}/MTPlanningScenery.h)


	################################## moc'ing ##############################
	set(GUI_MOC_HDRS
	    ${PROJECT_SOURCE_DIR}/MTPlanningWindow.h
	)
	
	set(GUI_UIS
	  ${PROJECT_SOURCE_DIR}/MTPlanning.ui
	)

  VirtualRobotQtApplication(${PROJECT_NAME} "${demo_SRCS}" "${demo_INCS}" "${GUI_MOC_HDRS}" "${GUI_UIS}")
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} PRIVATE Saba)

  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES FOLDER "Examples")
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${Simox_BIN_DIR})
	
  #######################################################################################
  ############################ Setup for installation ###################################
  #######################################################################################

  install(TARGETS ${PROJECT_NAME}
    # IMPORTANT: Add the library to the "export-set"
    EXPORT SimoxTargets
    RUNTIME DESTINATION bin COMPONENT bin
    COMPONENT dev)

  MESSAGE( STATUS " ** Simox application ${PROJECT_NAME} will be placed into " ${Simox_BIN_DIR})
  MESSAGE( STATUS " ** Simox application ${PROJECT_NAME} will be installed into " bin)


ENDIF()
