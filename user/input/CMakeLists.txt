aux_source_directory(. SRC)
add_executable(input_user ${SRC})

install(TARGETS input_user DESTINATION user)