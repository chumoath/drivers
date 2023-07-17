compile_module(led)

# 必须打开下面内容，编写 led.c 才会进行提示
aux_source_directory(. LED_SRC)
add_executable(led_exe ${LED_SRC})