find_package(Boost REQUIRED)

add_library(mathematics)
target_sources(mathematics PUBLIC FILE_SET CXX_MODULES FILES mathematics.ccm)
target_link_libraries(mathematics PRIVATE my_config)

add_executable(cpp_project main.cc)
target_link_libraries(
  cpp_project
  PRIVATE my_config
          Boost::system
          Boost::coroutine
          Boost::regex
          Boost::thread
          Boost::date_time
          Boost::serialization
          mathematics)
target_include_directories(cpp_project PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
