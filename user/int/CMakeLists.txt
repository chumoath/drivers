aux_source_directory(. SRC)
add_executable(int_user ${SRC})

install(TARGETS int_user DESTINATION user)