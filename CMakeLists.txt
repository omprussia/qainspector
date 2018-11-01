project(qainspector)

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
set(QT_MIN_VERSION "5.11.0")

################# set KDE specific information #################

find_package(ECM 5.40.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core DBus Quick QuickControls2 Network)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(ECMPackageConfigHelpers)
include(ECMOptionalAddSubdirectory)

include_directories(${Qt5Core_PRIVATE_INCLUDE_DIRS})

qt5_add_resources(${PROJECT_NAME}_SRCS qml.qrc)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS}
    dbusconnector.cpp
    main.cpp
    mytreemodel.cpp
    socketconnector.cpp
    main.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH ${CMAKE_INSTALL_FULL_LIBDIR}/${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "ru.omprussia.qainspector")

target_link_libraries(${PROJECT_NAME} LINK_PUBLIC Qt5::QuickControls2 Qt5::Network Qt5::Quick Qt5::DBus Qt5::Core)

install(TARGETS ${PROJECT_NAME} ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES "ru.omprussia.qainspector.desktop" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications")

set(ICON_SIZES 16 24 32 48 64 128 256 512)
foreach(size ${ICON_SIZES})
    set(path_from "icons/hicolor/${size}x${size}/apps/ru.omprussia.qainspector.png")
    set(path_to "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/${size}x${size}/apps/")
    install(FILES ${path_from} DESTINATION ${path_to})
endforeach(size)
