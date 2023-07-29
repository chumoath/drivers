set (ko_target led proc i2c common)
add_dependencies(led common)
add_dependencies(proc common)
add_dependencies(i2c common)