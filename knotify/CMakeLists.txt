add_subdirectory( sounds  )
########### next target ###############

set(knotify_SRCS
main.cpp
knotify.cpp
notifybysound.cpp
notifybypopup.cpp
notifybypopupgrowl.cpp
notifybylogfile.cpp
notifybytaskbar.cpp
notifybyexecute.cpp
notifybyktts.cpp
imageconverter.cpp
ksolidnotify.cpp
)

set(knotifyplugin_SRCS
knotifyplugin.cpp
knotifyconfig.cpp
)

set(knotifyplugin_HEADERS
knotifyplugin.h
knotifyconfig.h
knotify_export.h
)

qt4_add_dbus_interfaces(knotify_SRCS ${KDE4_DBUS_INTERFACES_DIR}/org.kde.KSpeech.xml)

kde4_add_app_icon(knotify_SRCS "${CMAKE_SOURCE_DIR}/pics/oxygen/*/apps/preferences-desktop-notification.png")

set (knotify_OUTPUT_NAME knotify4)
kde4_add_executable( knotify ${knotify_SRCS})

kde4_add_library( knotifyplugin SHARED ${knotifyplugin_SRCS})


if (Q_WS_MAC)
    set_target_properties(knotify PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/Info.plist.template)
    set_target_properties(knotify PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.kde.knotify4")
    set_target_properties(knotify PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "KDE Notify")
endif (Q_WS_MAC)

target_link_libraries( knotify ${KDE4_KDEUI_LIBS} ${KDE4_PHONON_LIBS} ${KDE4_SOLID_LIBS} knotifyplugin)

target_link_libraries( knotifyplugin ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS})


set_target_properties(knotify PROPERTIES OUTPUT_NAME knotify4 )
install(TARGETS knotify ${INSTALL_TARGETS_DEFAULT_ARGS} )
########### install files ###############

install( FILES kde.notifyrc  DESTINATION  ${DATA_INSTALL_DIR}/kde )
install( FILES hardwarenotifications.notifyrc DESTINATION ${DATA_INSTALL_DIR}/hardwarenotifications )
install( FILES knotify4.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )

install( FILES knotifynotifymethod.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR} )
install( TARGETS knotifyplugin ${INSTALL_TARGETS_DEFAULT_ARGS} )
install( FILES ${knotifyplugin_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR} )

########### D-Bus Autostart Services #########


configure_file(org.kde.knotify.service.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/org.kde.knotify.service)

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.knotify.service DESTINATION ${DBUS_SERVICES_INSTALL_DIR} )
