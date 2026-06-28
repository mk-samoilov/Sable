find_package(Vulkan REQUIRED)
find_package(Lua REQUIRED)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
  
add_subdirectory(libs/glfw)
add_subdirectory(libs/glm)

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
file(GLOB_RECURSE ENGINE_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)

add_library(engine SHARED
    ${ENGINE_SOURCES}
    ${ENGINE_HEADERS}
)

set_target_properties(engine PROPERTIES
    # PREFIX ""
    OUTPUT_NAME sable
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(engine PUBLIC -Wno-template-body)
endif()

target_include_directories(engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/stb
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/sol
    ${LUA_INCLUDE_DIR}
)

target_link_libraries(engine PUBLIC
    Threads::Threads
    Vulkan::Vulkan
    glfw
    glm::glm
    ${LUA_LIBRARIES}
)
