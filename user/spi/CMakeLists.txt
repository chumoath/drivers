aux_source_directory(. SRC)
add_executable(spi_user ${SRC})

install(TARGETS spi_user DESTINATION user)