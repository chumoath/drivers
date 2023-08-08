aux_source_directory(. SRC)
add_executable(i2c_user ${SRC})

install(TARGETS i2c_user DESTINATION user)