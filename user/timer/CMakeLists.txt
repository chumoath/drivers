aux_source_directory(. SRC)
add_executable(timer_user ${SRC})

install(TARGETS timer_user DESTINATION user)