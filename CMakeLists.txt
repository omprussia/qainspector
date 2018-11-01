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

qt5_add_resources(qainspector_SRCS qml.qrc)

add_executable(qainspector ${qainspector_SRCS}
    dbusconnector.cpp
    main.cpp
    mytreemodel.cpp
    socketconnector.cpp
    main.cpp
)

set_target_properties(qainspector PROPERTIES INSTALL_RPATH ${CMAKE_INSTALL_FULL_LIBDIR}/qainspector)

target_link_libraries(qainspector LINK_PUBLIC Qt5::QuickControls2 Qt5::Network Qt5::Quick Qt5::DBus Qt5::Core)

install(TARGETS qainspector ${INSTALL_TARGETS_DEFAULT_ARGS})
