add_executable(p4 p4.cpp)

set_target_properties(
  p4
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_link_libraries(p4 PRIVATE P4API::client)

if(MSVC)
  set_property(TARGET p4 PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
