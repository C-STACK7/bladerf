cmake_minimum_required(VERSION 3.16)

project(test_bladeinfo LANGUAGES C)

set(INCLUDES
    ${libbladeRF_SOURCE_DIR}/include
    ${libbladeRF_SOURCE_DIR}/src
    ${BLADERF_HOST_COMMON_INCLUDE_DIRS}
)

if(LIBBLADERF_SEARCH_PREFIX_OVERRIDE)
    add_definitions(-DLIBBLADERF_SEARCH_PREFIX="${LIBBLADERF_SEARCH_PREFIX_OVERRIDE}")
else()
    add_definitions(-DLIBBLADERF_SEARCH_PREFIX="${CMAKE_INSTALL_PREFIX}")
endif()

set(SRC
    main.c
)
add_executable(test_bladeinfo main.c)

include_directories(${INCLUDES})
#add_executable(test_bladeinfo ${SRC})
target_link_libraries(test_bladeinfo libbladerf_shared)

install(TARGETS test_bladeinfo
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)






