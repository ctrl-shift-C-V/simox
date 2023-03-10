PROJECT ( CameraViewer )

IF(Simox_VISUALIZATION AND Simox_USE_COIN_VISUALIZATION)

	# the variable "demo_SRCS" contains all .cpp files of this project
	FILE(GLOB demo_SRCS ${PROJECT_SOURCE_DIR}/CameraViewer.cpp ${PROJECT_SOURCE_DIR}/showCamWindow.cpp)
	FILE(GLOB demo_INCS ${PROJECT_SOURCE_DIR}/showCamWindow.h)


  ################################## moc'ing ##############################
  set(GUI_MOC_HDRS
      ${PROJECT_SOURCE_DIR}/showCamWindow.h
  )

  set(GUI_UIS
      ${PROJECT_SOURCE_DIR}/CameraViewer.ui
  )

  # create the executable
  VirtualRobotQtApplication(${PROJECT_NAME} "${demo_SRCS}" "${demo_INCS}" "${GUI_MOC_HDRS}" "${GUI_UIS}")

  if(WIN32)
        TARGET_LINK_LIBRARIES(${PROJECT_NAME} PRIVATE opengl32)
  endif(WIN32)

  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${Simox_BIN_DIR})
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES FOLDER "Examples")



    find_package(OpenGL REQUIRED)
    if(OPENGL_FOUND)
        target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${OPENGL_INCLUDE_DIR})
        TARGET_LINK_LIBRARIES(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES})
    endif(OPENGL_FOUND)




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
