project(ogs-dlls)

file(GLOB PROJECT_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(${PROJECT_NAME} MODULE ${PROJECT_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "server"
)

target_link_libraries(${PROJECT_NAME} ogs-interface ogs-mathlib ogs-pm_shared)