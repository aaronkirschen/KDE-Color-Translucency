set(kcm_SRCS
    ColorTranslucencyKCM.cpp
    plugin.cpp
)

kconfig_add_kcfg_files(kcm_SRCS ../ColorTranslucencyConfig.kcfgc)

qt6_wrap_ui(kcm_SRCS ColorTranslucencyKCM.ui)
qt6_add_dbus_interface(kcm_SRCS ${KWIN_EFFECTS_INTERFACE} kwineffects_interface)
kcoreaddons_add_plugin(kwin_colortranslucency_config INSTALL_NAMESPACE "kwin/effects/configs" SOURCES ${kcm_SRCS})

target_link_libraries(kwin_colortranslucency_config
        Qt${QT_MAJOR_VERSION}::CorePrivate
        KF${QT_MAJOR_VERSION}::KCMUtils
)

target_link_libraries(kwin_colortranslucency_config
    Qt${QT_MAJOR_VERSION}::DBus
    KF${QT_MAJOR_VERSION}::ConfigWidgets
)

