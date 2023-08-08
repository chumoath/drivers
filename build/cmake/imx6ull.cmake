set (ko_target
        led
        proc
        i2c
        common
        timer
        atomic
        spinlock
        semaphore
        mutex
        int
        blockio
        noblockio
        asyncnoti
        platform
        platform_dts
        misc
        input
        rtc
        spi)


add_dependencies(led common)
add_dependencies(proc common)
add_dependencies(i2c common)
add_dependencies(timer common)
add_dependencies(atomic common)
add_dependencies(spinlock common)
add_dependencies(semaphore common)
add_dependencies(mutex common)
add_dependencies(int common)
add_dependencies(blockio common)
add_dependencies(noblockio common)
add_dependencies(asyncnoti common)
add_dependencies(platform common)
add_dependencies(platform_dts common)
add_dependencies(misc common)
add_dependencies(input common)
add_dependencies(rtc common)
add_dependencies(i2c common)
add_dependencies(spi common)