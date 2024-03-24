add_subdirectory(kcm)

set(effect_SRCS
    ColorTranslucencyEffect.cpp
    ColorTranslucencyShader.cpp
    plugin.cpp
)

kconfig_add_kcfg_files(effect_SRCS ColorTranslucencyConfig.kcfgc)
kcoreaddons_add_plugin(kwin4_effect_colortranslucency SOURCES ${effect_SRCS} INSTALL_NAMESPACE "kwin/effects/plugins/")

target_link_libraries(kwin4_effect_colortranslucency
        Qt${QT_MAJOR_VERSION}::DBus
        KF${QT_MAJOR_VERSION}::ConfigGui
)

target_link_libraries(kwin4_effect_colortranslucency
        KWin::kwin
        Qt${QT_MAJOR_VERSION}::CorePrivate
)

install(DIRECTORY shaders DESTINATION ${KDE_INSTALL_DATADIR}/kwin)