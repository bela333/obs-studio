project(obs-equi)


configure_file("${CMAKE_CURRENT_SOURCE_DIR}/obs-filters-config.h.in"
	"${CMAKE_BINARY_DIR}/plugins/obs-equi/config/obs-filters-config.h")

set(obs-equi_config_HEADERS
	"${CMAKE_BINARY_DIR}/plugins/obs-equi/config/obs-filters-config.h")
include_directories("${CMAKE_BINARY_DIR}/plugins/obs-equi/config")

if(MSVC)
	set(obs-equi_PLATFORM_DEPS
		w32-pthreads)
endif()

set(obs-equi_SOURCES
	obs-equi.c
	equiRotate-filter.c
	equiConvert-filter.c)

add_library(obs-equi MODULE
	${obs-equi_SOURCES}
	${obs-equi_config_HEADERS})
target_link_libraries(obs-equi
	libobs
	${obs-equi_PLATFORM_DEPS})

install_obs_plugin_with_data(obs-equi data)
