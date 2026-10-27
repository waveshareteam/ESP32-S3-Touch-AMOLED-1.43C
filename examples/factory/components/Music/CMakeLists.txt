idf_component_register(SRCS "Music.cpp" "assets/img_app_musicplayer.c" "assets/img_app_music_cover_1.c"
 INCLUDE_DIRS "." REQUIRES bsp_extra app_support EMBED_FILES "audio/demo.pcm")
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-missing-field-initializers)
