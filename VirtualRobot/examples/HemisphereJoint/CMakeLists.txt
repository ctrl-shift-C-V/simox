project(HemisphereJoint)

find_package(Boost ${Simox_BOOST_VERSION} EXACT COMPONENTS system REQUIRED)

if (Boost_FOUND)
    set(SOURCES
        main.cpp
    )
    set(HEADERS
    )

    set(LIBS
        VirtualRobot
        ${BOOST_FILESYSTEM_LIBRARIES}
        ${BOOST_SYSTEM_LIBRARIES}
    )

    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${Simox_BIN_DIR})
    set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Examples")

    # against undefined reference to boost::filesystem::detail::copy_file
    # source: https://stackoverflow.com/a/3500721
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DBOOST_NO_CXX11_SCOPED_ENUMS)

    target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})

    #######################################################################################
    ############################ Setup for installation ###################################
    #######################################################################################
    install(TARGETS ${PROJECT_NAME}
        # IMPORTANT: Add the library to the "export-set"
        EXPORT SimoxTargets
        RUNTIME DESTINATION bin COMPONENT bin
        COMPONENT dev)

    message( STATUS " ** Simox application ${PROJECT_NAME} will be placed into " ${Simox_BIN_DIR})
    message( STATUS " ** Simox application ${PROJECT_NAME} will be installed into " bin)
endif()
