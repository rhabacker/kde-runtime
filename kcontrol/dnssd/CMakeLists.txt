


########### next target ###############

set(kcm_kdnssd_PART_SRCS kcmdnssd.cpp kcmdnssd.h )


kde4_add_ui_files(kcm_kdnssd_PART_SRCS configdialog.ui )

kde4_add_plugin(kcm_kdnssd ${kcm_kdnssd_PART_SRCS})


target_link_libraries(kcm_kdnssd  ${KDE4_KDNSSD_LIBS} ${KDE4_KDEUI_LIBS} )

install(TARGETS kcm_kdnssd  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kcm_kdnssd.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
