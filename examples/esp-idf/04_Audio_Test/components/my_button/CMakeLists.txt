idf_component_register(
  SRCS "button.cc"
  INCLUDE_DIRS "./"
  REQUIRES driver espressif__button
)
#add_compile_definitions(XPOWERS_CHIP_AXP2101 CONFIG_XPOWERS_ESP_IDF_NEW_API)
##REQUIRES