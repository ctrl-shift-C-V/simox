PROJECT ( GraspRrtDemo )

IF(Simox_VISUALIZATION AND Simox_USE_COIN_VISUALIZATION)
	
  # the variable "demo_SRCS" contains all .cpp files of this project
  FILE(GLOB demo_SRCS ${PROJECT_SOURCE_DIR}/GraspRrtDemo.cpp ${PROJECT_SOURCE_DIR}/GraspRrtWindow.cpp)
  FILE(GLOB demo_INCS ${PROJECT_SOURCE_DIR}/GraspRrtWindow.h)


  ################################## moc'ing ##############################
  set(GUI_MOC_HDRS
      ${PROJECT_SOURCE_DIR}/GraspRrtWindow.h
  )

  set(GUI_UIS
      ${PROJECT_SOURCE_DIR}/GraspRrt.ui
  )

  SimoxQtApplication("${PROJECT_NAME}" "${demo_SRCS}" "${demo_INCS}" "${GUI_MOC_HDRS}" "${GUI_UIS}")

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
