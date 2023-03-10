PROJECT ( SimDynamicsViewer )

IF(Simox_USE_COIN_VISUALIZATION)

    FILE(GLOB demo_SRCS ${PROJECT_SOURCE_DIR}/SimDynamicsViewer.cpp ${PROJECT_SOURCE_DIR}/simDynamicsWindow.cpp)
    FILE(GLOB demo_INCS ${PROJECT_SOURCE_DIR}/simDynamicsWindow.h)

    set(GUI_MOC_HDRS
        ${PROJECT_SOURCE_DIR}/simDynamicsWindow.h
    )

    set(GUI_UIS
        ${PROJECT_SOURCE_DIR}/simDynamicsViewer.ui
    )

    SimoxQtApplication("${PROJECT_NAME}" "${demo_SRCS}" "${demo_INCS}" "${GUI_MOC_HDRS}" "${GUI_UIS}")
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} PRIVATE SimDynamics)

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
