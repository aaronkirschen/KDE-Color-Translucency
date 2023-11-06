add_subdirectory(kcm)

set(effect_SRCS
    ColorTranslucencyEffect.cpp
    ColorTranslucencyShader.cpp
    plugin.cpp
)

kconfig_add_kcfg_files(effect_SRCS ColorTranslucencyConfig.kcfgc)
add_library(kwin4_effect_colortranslucency SHARED ${effect_SRCS})

target_link_libraries(kwin4_effect_colortranslucency
    Qt5::Core
    Qt5::Gui
    Qt5::DBus
    KWinEffects::kwineffects
    KWinEffects::kwinglutils
    epoxy::epoxy
    KF5::ConfigCore
    KF5::ConfigGui
    KF5::CoreAddons
    KF5::WindowSystem
)

install(TARGETS kwin4_effect_colortranslucency DESTINATION ${PLUGIN_INSTALL_DIR}/kwin/effects/plugins/)

execute_process(COMMAND kf5-config --install data OUTPUT_VARIABLE DATAPATH OUTPUT_STRIP_TRAILING_WHITESPACE)
install(FILES shaders/colortranslucency.frag DESTINATION ${DATAPATH}/kwin/shaders/)
install(FILES shaders/colortranslucency_core.frag DESTINATION ${DATAPATH}/kwin/shaders/)