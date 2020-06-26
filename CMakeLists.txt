cmake_minimum_required(VERSION 2.8)

project(Opentrea)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# On MSYS2, building a stand-alone binary that links with ffmpeg is not possible,
# Therefore, ffmpeg, libx264 and libx265 must be build from source
if(WIN32)
	option(OPENTREA_STANDALONE "Compile stand-alone binary of Opentrea" OFF)
	if(OPENTREA_STANDALONE)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
	
		if(NOT DEFINED OPENTREA_PREPARED_BINARIES)
			set(OPENTREA_PREPARED_BINARIES "${CMAKE_CURRENT_SOURCE_DIR}/pre-compiled/windows")
		endif()
		list(PREPEND PLATFORM_LIBRARIES
			C:/msys64/mingw64/lib/gcc/x86_64-w64-mingw32/${CMAKE_CXX_COMPILER_VERSION}/libstdc++.a
			C:/msys64/mingw64/x86_64-w64-mingw32/lib/libwinpthread.a
			)

		set(FFMPEG_INCLUDE_DIRS
			${OPENTREA_PREPARED_BINARIES}/include)
		set(FFMPEG_LIBRARIES
			${OPENTREA_PREPARED_BINARIES}/lib/libavcodec.a
			${OPENTREA_PREPARED_BINARIES}/lib/libavdevice.a
			${OPENTREA_PREPARED_BINARIES}/lib/libavfilter.a
			${OPENTREA_PREPARED_BINARIES}/lib/libavformat.a
			${OPENTREA_PREPARED_BINARIES}/lib/libavutil.a
			${OPENTREA_PREPARED_BINARIES}/lib/libpostproc.a
			${OPENTREA_PREPARED_BINARIES}/lib/libswresample.a
			${OPENTREA_PREPARED_BINARIES}/lib/libswscale.a
			${OPENTREA_PREPARED_BINARIES}/lib/libx264.a
			${OPENTREA_PREPARED_BINARIES}/lib/libx265.a
			z lzma bcrypt C:/msys64/mingw64/lib/libiconv.a)
	endif()
else()
	set(OPENTREA_STANDALONE OFF)
endif()

add_subdirectory(Simple-Web-Server)
add_subdirectory(moonlight-common-c/enet)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

if(NOT OPENTREA_STANDALONE)
	find_package(FFmpeg REQUIRED)
endif()

list(APPEND OPENTREA_COMPILE_OPTIONS -fPIC -Wall -Wno-missing-braces -Wno-maybe-uninitialized -Wno-sign-compare)

if(WIN32)
	add_subdirectory(tools) #This is temporary, only tools for Windows are needed, for now

	list(APPEND OPENTREA_DEFINITIONS APPS_JSON="apps_windows.json")
	include_directories(
		ViGEmClient/include)
	set(PLATFORM_TARGET_FILES
		opentrea/platform/windows/input.cpp
		opentrea/platform/windows/display.h
		opentrea/platform/windows/display_base.cpp
		opentrea/platform/windows/display_vram.cpp
		opentrea/platform/windows/display_ram.cpp
		opentrea/platform/windows/audio.cpp
		ViGEmClient/src/ViGEmClient.cpp
		ViGEmClient/include/ViGEm/Client.h
		ViGEmClient/include/ViGEm/Common.h
		ViGEmClient/include/ViGEm/Util.h
		ViGEmClient/include/ViGEm/km/BusShared.h)
	list(PREPEND PLATFORM_LIBRARIES
		winmm
		ksuser
		wsock32
		ws2_32
		iphlpapi
		d3d11 dxgi
		setupapi
		)

	set_source_files_properties(ViGEmClient/src/ViGEmClient.cpp PROPERTIES COMPILE_DEFINITIONS "UNICODE=1;ERROR_INVALID_DEVICE_OBJECT_PARAMETER=650")
	set_source_files_properties(ViGEmClient/src/ViGEmClient.cpp PROPERTIES COMPILE_FLAGS "-Wno-unknown-pragmas -Wno-misleading-indentation -Wno-class-memaccess")
else()
	list(APPEND OPENTREA_DEFINITIONS APPS_JSON="apps_linux.json")

	find_package(X11 REQUIRED)
	set(PLATFORM_TARGET_FILES
		opentrea/platform/linux/display.cpp
		opentrea/platform/linux/input.cpp)
	
	set(PLATFORM_LIBRARIES
		Xfixes
		Xtst
		xcb
		xcb-shm
		xcb-xfixes
		${X11_LIBRARIES}
		evdev
		pulse
		pulse-simple
		)
	
	set(PLATFORM_INCLUDE_DIRS
		${X11_INCLUDE_DIR}
		/usr/include/libevdev-1.0)

	if(NOT DEFINED OPENTREA_EXECUTABLE_PATH)
		set(OPENTREA_EXECUTABLE_PATH "${CMAKE_CURRENT_BINARY_DIR}/opentrea")
	endif()
	configure_file(gen-deb.in gen-deb @ONLY)
	configure_file(opentrea.service.in opentrea.service @ONLY)
endif()

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS log filesystem REQUIRED)

set(OPENTREA_TARGET_FILES
	moonlight-common-c/reedsolomon/rs.c
	moonlight-common-c/reedsolomon/rs.h
	moonlight-common-c/src/Input.h
	moonlight-common-c/src/Rtsp.h
	moonlight-common-c/src/RtspParser.c
	moonlight-common-c/src/Video.h
	opentrea/utility.h
	opentrea/uuid.h
	opentrea/config.h
	opentrea/config.cpp
	opentrea/main.cpp
	opentrea/main.h
	opentrea/crypto.cpp
	opentrea/crypto.h
	opentrea/nvhttp.cpp
	opentrea/nvhttp.h
	opentrea/rtsp.cpp
	opentrea/rtsp.h
	opentrea/stream.cpp
	opentrea/stream.h
	opentrea/video.cpp
	opentrea/video.h
	opentrea/input.cpp
	opentrea/input.h
	opentrea/audio.cpp
	opentrea/audio.h
	opentrea/platform/common.h
	opentrea/process.cpp
	opentrea/process.h
	opentrea/network.cpp
	opentrea/network.h
	opentrea/move_by_copy.h
	opentrea/task_pool.h
	opentrea/thread_pool.h
	opentrea/thread_safe.h
	opentrea/sync.h
	opentrea/round_robin.h
	${PLATFORM_TARGET_FILES})

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Simple-Web-Server
  ${CMAKE_CURRENT_SOURCE_DIR}/moonlight-common-c/enet/include
  ${CMAKE_CURRENT_SOURCE_DIR}/moonlight-common-c/reedsolomon
  ${FFMPEG_INCLUDE_DIRS}
  ${PLATFORM_INCLUDE_DIRS}
)

string(TOUPPER "x${CMAKE_BUILD_TYPE}" BUILD_TYPE)
if("${BUILD_TYPE}" STREQUAL "XDEBUG")
	list(APPEND OPENTREA_COMPILE_OPTIONS -O0 -pedantic -ggdb3)
	if(WIN32)
		set_source_files_properties(opentrea/nvhttp.cpp PROPERTIES COMPILE_FLAGS -O2)
	endif()
else()
	add_definitions(-DNDEBUG)
	list(APPEND OPENTREA_COMPILE_OPTIONS -O3)
endif()

if(NOT OPENTREA_ASSETS_DIR)
	set(OPENTREA_ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
endif()

if(OPENTREA_STANDALONE)
	set(OPENSSL_LIBRARIES
		C:/msys64/mingw64/lib/libssl.a
		C:/msys64/mingw64/lib/libcrypto.a)
endif()

list(APPEND OPENTREA_EXTERNAL_LIBRARIES
		${CMAKE_THREAD_LIBS_INIT}
		stdc++fs
		enet
		opus
		${FFMPEG_LIBRARIES}
		${Boost_LIBRARIES}
		${OPENSSL_LIBRARIES}
		${PLATFORM_LIBRARIES})

list(APPEND OPENTREA_DEFINITIONS OPENTREA_ASSETS_DIR="${OPENTREA_ASSETS_DIR}")
add_executable(opentrea ${OPENTREA_TARGET_FILES})
target_link_libraries(opentrea ${OPENTREA_EXTERNAL_LIBRARIES})
target_compile_definitions(opentrea PUBLIC ${OPENTREA_DEFINITIONS})
set_target_properties(opentrea PROPERTIES CXX_STANDARD 17)

target_compile_options(opentrea PRIVATE ${OPENTREA_COMPILE_OPTIONS})
