aux_source_directory(. SRC)
add_executable(noblockio_user ${SRC})

install(TARGETS noblockio_user DESTINATION user)