cmake_minimum_required(VERSION 2.8)

project(GlobalGameJam2018)

SET(OPIFEX_APPNAME "GlobalGameJam2018")

# Sets up all of your application needs
# it then sets up all of the OPifex Engine links
include(CMake/application.cmake)

add_marketplace_addons_projects()


#########################################
# LAYER 9 - Application
#########################################

# Include Directories
include_directories(
	${PROJECT_SOURCE_DIR}
	.
	..
)

# Include directories for the engine itself
# Plus all of the external libraries needed
add_external_opifex_includes()


# Application Source Files
# If you change the src folder or add another, you'll need to change it here
add_source_group_recurse("${PROJECT_SOURCE_DIR}/src" "" APPLICATION_SRC)
add_asset_dir(APPLICATION_SRC)


# Application BUILD
add_opifex_executable(9_Application ${APPLICATION_SRC})

add_opifex_definitions(9_Application "")
add_marketplace_addons(9_Application "${BINARY_RELEASE_MODE}/" OPIFEX_ASSETS_ADDITIONAL)
add_opifex_defines()
set_target_properties(9_Application PROPERTIES OUTPUT_NAME "${OPIFEX_APPNAME}")
add_opifex_libraries(9_Application)
add_opifex_assets(9_Application)


opifex_engine_status_messages()
