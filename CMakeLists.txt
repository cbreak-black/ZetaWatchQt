cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

project(ZetaWatchQt)

################################################################################
# Settings
################################################################################

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

################################################################################
# Dependencies
################################################################################

add_subdirectory(ZFSWrapper)

find_package(Qt5 REQUIRED COMPONENTS Gui Widgets)

################################################################################
# Program
################################################################################

set(ZETA_WATCH_SOURCES
	src/main.cpp
)

qt_add_resources(ZETA_WATCH_RES media/ZetaIcon.qrc)

add_executable(${PROJECT_NAME} ${ZETA_WATCH_SOURCES} ${ZETA_WATCH_RES})

target_link_libraries(${PROJECT_NAME}
	ZFSWrapper
	Qt5::Widgets Qt5::Gui
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_options(${PROJECT_NAME} PRIVATE
	-Wall -Wextra -pedantic -Werror
	-Wno-error=unknown-pragmas
	-Wno-missing-field-initializers
)

# Organization in Xcode and Co
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ZETA_WATCH_SOURCES})
