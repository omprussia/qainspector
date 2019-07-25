cmake_minimum_required(VERSION 3.1)

project(qainspector LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_MIN_VERSION "5.12.0")
set(CMAKE_BUILD_TYPE MinSizeRel)

find_package(ECM 5.40.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core Quick QuickControls2 Network Xml XmlPatterns)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(CMakePackageConfigHelpers)
include(ECMOptionalAddSubdirectory)

add_executable(${PROJECT_NAME}
    "qml.qrc"
    "main.cpp"
    "mytreemodel.cpp"
    "socketconnector.cpp"
)

set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH ${CMAKE_INSTALL_FULL_LIBDIR}/${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "ru.omprussia.qainspector")

target_link_libraries(${PROJECT_NAME} LINK_PUBLIC Qt5::QuickControls2 Qt5::Network Qt5::Quick Qt5::Core Qt5::Xml Qt5::XmlPatterns)

install(TARGETS ${PROJECT_NAME} ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES "ru.omprussia.qainspector.desktop" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications")

set(ICON_SIZES 16 24 32 48 64 128 256 512)
foreach(size ${ICON_SIZES})
    set(path_from "icons/hicolor/${size}x${size}/apps/ru.omprussia.qainspector.png")
    set(path_to "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/${size}x${size}/apps/")
    install(FILES ${path_from} DESTINATION ${path_to})
endforeach(size)
