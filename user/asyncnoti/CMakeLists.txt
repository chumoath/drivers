aux_source_directory(. SRC)
add_executable(asyncnoti_user ${SRC})

install(TARGETS asyncnoti_user DESTINATION user)