cmake_minimum_required(VERSION 3.29)
project(ivc C)

set(CMAKE_C_STANDARD 99)

find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBZIP REQUIRED libzip)
pkg_check_modules(LIBYAML REQUIRED yaml-0.1)

add_executable(ivc main.c config_utils.c file_utils.c template_utils.c unzip_utils.c)

target_include_directories(ivc PRIVATE ${LIBZIP_INCLUDE_DIRS} ${LIBYAML_INCLUDE_DIRS})
target_link_libraries(ivc ${LIBZIP_LIBRARIES} ${LIBYAML_LIBRARIES})