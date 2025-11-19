idf_component_register(SRCS "hdc1080.cpp" INCLUDE_DIRS "." REQUIRES driver esp_timer)
target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++23)