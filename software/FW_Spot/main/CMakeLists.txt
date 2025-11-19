idf_component_register(SRCS "env_eye_lora.cpp" "env_eye_main.cpp"
                       PRIV_REQUIRES nano_esp32 hdc1080 wf183de syp16 esp_adc lora
                       INCLUDE_DIRS "")

target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++23)
