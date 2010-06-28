cmake_minimum_required (VERSION 2.8)
# The name of our project is "CPPCORE". CMakeLists files in this project can
# refer to the root source directory of the project as ${CPPCORE_SOURCE_DIR} and
# to the root binary directory of the project as ${CPPCORE_BINARY_DIR}.
project (CPPCORE)


#user customizable settings:

#uncomment to compile with OpenGLES v1.1 on linux. Defaults to yes on iPhone.
#SET(OPENGL_CLIENT YES)
#uncomment to use PAL text renering instead of os native/pango where available:
#SET(OPENGL_PAL_TEXT_RENDERING YES)

#debug building. Uncomment to make a release build:
SET(DEBUG_BUILD YES)

#build for iPhone Simulator? uncomment to build for device:
SET(XCODE_SIMULATOR YES)
#paths for iphone SDKs
SET(IPSIMSTRING iphonesimulator3.1.3)
SET(IPSIMSDK "/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator3.1.3.sdk")
SET(IPDEVSTRING iphoneos3.1.3)
SET(IPDEVSDK "/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS3.1.3.sdk/")

#build for maemo instead of regular linux:
#SET(MAEMO YES)

#uncomment to build WFAPITestClient (if linux build) linked to MapLibNG:
#SET(LINKWITH_MAPLIBNG YES)

#end of user customizable settings


SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNAV2_USE_UTF8")

IF(DEBUG_BUILD)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -D NAV2_CLIENT_DEBUG -D NAV2_ENABLE_STRESS_ASSERTS")
ELSE(DEBUG_BUILD)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
ENDIF(DEBUG_BUILD)

IF(UNIX AND NOT XCODE)
  IF(MAEMO)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D NAV2_CLIENT_NOKIAMAEMO")
  ELSE(MAEMO)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D NAV2_CLIENT_I386")
  ENDIF(MAEMO)
ENDIF(UNIX AND NOT XCODE)

IF(XCODE)
  SET(OPENGL_CLIENT YES)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG -no-cpp-precomp -DEXPORT_API=\"\" -DIPHONE -D__iPhone__ -DHAVE_ZLIB -D NAV2_CLIENT_IPHONE -mno-thumb")

  IF(XCODE_SIMULATOR)
    #iphone simulator:
    SET(CMAKE_OSX_ARCHITECTURES i386)
    SET(XCODE_ATTRIBUTE_SDKROOT ${IPSIMSTRING})
    SET(CMAKE_OSX_SYSROOT ${IPSIMSDK})
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-stack-protector")
  ELSE(XCODE_SIMULATOR)
    #iphone device
    SET(XCODE_ATTRIBUTE_SDKROOT ${IPDEVSTRING})
    SET(CMAKE_OSX_SYSROOT ${IPDEVSDK})
  ENDIF(XCODE_SIMULATOR)

ENDIF(XCODE)

SET( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_mod" ${CMAKE_MODULE_PATH}) 

#Text rendering
IF(OPENGL_PAL_TEXT_RENDERING)
  SET(OPENGL_CLIENT YES)
  FIND_PACKAGE(Freetype)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOPENGL_PAL_TEXT_RENDERING")
ENDIF(OPENGL_PAL_TEXT_RENDERING)

#enable opengl defines
IF(OPENGL_CLIENT)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_OPENGL_ES")
ENDIF(OPENGL_CLIENT)

#add libs z, gtk and threads if needed
IF(NOT SYMBIAN)
  FIND_PACKAGE(ZLIB)
  IF(NOT XCODE)
    FIND_PACKAGE(GTK2 REQUIRED gtk)
    INCLUDE_DIRECTORIES(${GTK2_INCLUDE_DIRS})
    FIND_PACKAGE(Threads)
  ENDIF(NOT XCODE)
ENDIF(NOT SYMBIAN)

#Below are the subdirectories for the modules.
add_subdirectory (cpp/Modules/Common)
add_subdirectory (cpp/Modules/CtrlHub)
add_subdirectory (cpp/Modules/Destinations)
add_subdirectory (cpp/Modules/FakeNavServerCom)
add_subdirectory (cpp/Modules/GpsSirf)
add_subdirectory (cpp/Modules/NavServerCom)
add_subdirectory (cpp/Modules/NavTask)
add_subdirectory (cpp/Modules/Parameter)
add_subdirectory (cpp/Modules/QueueSerial)
add_subdirectory (cpp/Modules/UiCtrl)

add_subdirectory (cpp/Shared)
add_subdirectory (cpp/Targets/CommonLib)
add_subdirectory (cpp/Targets/Nav2API)
add_subdirectory (cpp/Targets/Nav2lib)
add_subdirectory (cpp/Targets/MapLib)
add_subdirectory (cpp/Targets/MapLibNG)

IF(SYMBIAN)
  add_subdirectory (cpp/Modules/TcpSymbianSerial/symbian-r6)
  add_subdirectory (cpp/Targets/OggVorbisLib)
ENDIF(SYMBIAN)

IF(NOT SYMBIAN)
  add_subdirectory (cpp/Targets/AppExamples)
  add_subdirectory (cpp/Targets/GUBI)
  add_subdirectory (cpp/Targets/GUBIWidget)
  add_subdirectory (cpp/Modules/TcpSerial)
  add_subdirectory (cpp/Targets/PAL)
  add_subdirectory (cpp/Targets/WFAPITestClient)
  add_subdirectory (cpp/Targets/RegressionTests)
  add_subdirectory (cpp/Targets/RegressionTestsNG)
ENDIF(NOT SYMBIAN)


