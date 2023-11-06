set(kcm_SRCS
    ColorTranslucencyKCM.cpp
    plugin.cpp
)

kconfig_add_kcfg_files(kcm_SRCS ../ColorTranslucencyConfig.kcfgc)

qt5_wrap_ui(kcm_SRCS ColorTranslucencyKCM.ui)

qt5_add_dbus_interface(kcm_SRCS ${KWIN_EFFECTS_INTERFACE} kwineffects_interface)

add_library(kwin_colortranslucency_config MODULE ${kcm_SRCS})

target_link_libraries(kwin_colortranslucency_config
    Qt5::Core
    Qt5::DBus
    Qt5::Gui
    KF5::ConfigWidgets
    KF5::ConfigGui
    KF5::GlobalAccel
    KF5::WindowSystem
)

install(TARGETS kwin_colortranslucency_config DESTINATION ${PLUGIN_INSTALL_DIR}/kwin/effects/configs)