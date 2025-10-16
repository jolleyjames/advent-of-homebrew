cmake_minimum_required(VERSION 3.18)

project(advhb LANGUAGES CXX VERSION 1.0.0)

find_library(FAT32_LIBRARY NAMES fat HINTS "${DEVKITPRO}/libogc/lib/wii")

find_package(Eigen3 REQUIRED NO_MODULE HINTS "/usr/local/share/eigen3/cmake")

file(GLOB SOLUTION_SOURCES "source/solutions/*.cpp")
add_library(Solutions OBJECT)
target_sources(Solutions PRIVATE ${SOLUTION_SOURCES} source/solution.cpp)
target_compile_options(Solutions PRIVATE -Wall)
target_include_directories(Solutions PRIVATE include)
target_link_libraries(Solutions PRIVATE Eigen3::Eigen)

add_library(Menu OBJECT)
target_sources(Menu PRIVATE source/menu.cpp)
target_compile_options(Menu PRIVATE -Wall)
target_include_directories(Menu PRIVATE include)

add_executable(boot)
target_sources(boot PRIVATE source/main.cpp)
target_compile_options(boot PRIVATE -Wall)
target_include_directories(boot PRIVATE include)
target_link_libraries(boot PRIVATE Menu Solutions ${FAT32_LIBRARY})

ogc_create_dol(boot)

add_custom_target(zip
  COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_NAME}/input"
  COMMAND ${CMAKE_COMMAND} -E copy boot.dol "${PROJECT_SOURCE_DIR}/meta.xml" "${PROJECT_SOURCE_DIR}/icon.png" ${PROJECT_NAME}
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/input-test" "${PROJECT_NAME}/input-test"
  COMMAND ${CMAKE_COMMAND} -E tar cf "${PROJECT_NAME}.zip" --format=zip ${PROJECT_NAME}
  COMMAND ${CMAKE_COMMAND} -E rm -rf ${PROJECT_NAME}
)
