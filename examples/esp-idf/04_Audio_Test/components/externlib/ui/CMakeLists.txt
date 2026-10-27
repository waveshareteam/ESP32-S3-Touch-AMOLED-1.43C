set(PROJ_SRC ${CMAKE_CURRENT_LIST_DIR})

file(GLOB_RECURSE PROJ_SRCS_C ${PROJ_SRC}/*.c)
file(GLOB_RECURSE PROJ_SRCS_CPP ${PROJ_SRC}/*.cpp)

idf_component_register(
    SRCS  
    ${PROJ_SRCS_C} 
    ${PROJ_SRCS_CPP}
    INCLUDE_DIRS 
    ${PROJ_SRC}
    PRIV_REQUIRES
    lvgl
)
#spiffs_create_partition_image(storage ${CMAKE_CURRENT_SOURCE_DIR}/pcm FLASH_IN_PROJECT)
#target_compile_definitions(${COMPONENT_LIB} PRIVATE LV_LVGL_H_INCLUDE_SIMPLE)
