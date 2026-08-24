# CMakeList.txt : CMake project for 2D, include source and define
# project specific logic here.
#
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(glm REQUIRED)
# Add source to this project's executable.
add_executable (2D "2D.cpp" "timer.h" "Animation.h" "gameObject.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 2D PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(2D PRIVATE SDL3::SDL3 SDL3_image::SDL3_image glm::glm)
