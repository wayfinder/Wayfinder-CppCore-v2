# Create a library called "RegTestsNG"

include_directories(
  include
  ../PAL/include
  ../Nav2API/Shared/include
  ../RegressionTests/include
  ../RegressionTests/Export/include/Shared
  ../Nav2API/Export/include/
  ../CommonLib/include
  ../CommonLib/POSIX/include
  ../Nav2API/Export/include/Shared
  ../../include/GuiProt
  ../../include/CombinedSearch
  ../../Modules/UiCtrl
  ../../Modules/include
  ../Nav2lib
  ../MapLibNG/Shared/include
  ../MapLibNG/Export/include/Shared
  ../MapLibNG/Export/internal/include
  ../MapLibNG/Gtk/include/
  ../Nav2API/Export/include/Unix
  ../MapLibNG/Export/include/Unix
  ../MapLibNG/Unix/include
  ../Nav2API/Unix/include
  ../AppExamples/Shared/include/
  ../GUBI/include/
  ../FMath/include/
  ../../arch-include/i386-redhat-linux/
  ../../include
)

IF(XCODE)
  include_directories(
    iPhone/include/
    ../CommonLib/iPhone/include/
    ../MapLibNG/Export/include/iPhone/
    ../Nav2API/Export/include/iPhone/
    ../Nav2API/iPhone/include/
  )
  IF(XCODE_SIMULATOR)
    #ugly, yes:
    include_directories(
      ${IPSIMSDK}/System/Library/Frameworks/CFNetwork.framework/Headers
    )
  ELSE(XCODE_SIMULATOR)
    include_directories(
      ${IPDEVSDK}/System/Library/Frameworks/CFNetwork.framework/Headers
    )
  ENDIF(XCODE_SIMULATOR)
  AUX_SOURCE_DIRECTORY(iPhone/src/ CUR_DIR_SOURCEFILES)
ENDIF(XCODE)

AUX_SOURCE_DIRECTORY(src/ CUR_DIR_SOURCEFILES)
AUX_SOURCE_DIRECTORY(. MAIN_SOURCEFILES)
AUX_SOURCE_DIRECTORY(../CommonLib/POSIX/src/ CUR_DIR_SOURCEFILES)
AUX_SOURCE_DIRECTORY(../CommonLib/POSIX/src/JTC/Tests/ CUR_DIR_SOURCEFILES)

IF(OPENGL_CLIENT OR XCODE)
  AUX_SOURCE_DIRECTORY(src/GUBI/ CUR_DIR_SOURCEFILES)
ENDIF(OPENGL_CLIENT OR XCODE)

IF(UNIX AND OPENGL_CLIENT)
  set (OPENGL_LIBS -lEGL -lGLES_CM)
ENDIF(UNIX AND OPENGL_CLIENT)


set (REGTESTSNG_LIBS "RegTestsNG")
set (MAPLIBNG_LIBS "MapLibNG")
set (APPEXAMPLE_LIBS "AppExamples")
set (PALLIB_LIBS "PAL")
set (GUBI_LIBS "GUBI")
set (NAV2API_LIBS "Nav2API")
set (NAV2LIB_LIBS "Nav2lib")
set (COMMONLIB_LIBS "CommonLib")
set (DESTMOD_LIBS "DestinationsModule")
set (UICTRLMOD_LIBS "UiCtrlModule")
set (COMBSEARCH_LIBS "CombinedSearch")
set (FAKENAVSRVCOMMOD_LIBS "FakeNavServerComModule")
set (PARAMMOD_LIBS "ParameterModule")
set (GPSSIRFMOD_LIBS "GpsSirfModule")
set (QSMOD_LIBS "QueueSerialModule")
set (COMMONMOD_LIBS "CommonModule")
set (GUIPROT_LIBS "GuiProt")
set (NAVSRVCOMMOD_LIBS "NavServerComModule")
set (SHARED_LIBS "Shared")
set (CTRLHUBMOD_LIBS "CtrlHubModule")
set (NAVTASK_LIBS "NavTask")
set (TCPSERIALMOD_LIBS "TcpSerialModule")

add_library(RegTestsNG ${CUR_DIR_SOURCEFILES})

IF(NOT XCODE)
add_executable (RegressionTesterNG ${MAIN_SOURCEFILES})
target_link_libraries(RegressionTesterNG 
 ${REGTESTSNG_LIBS}
 ${ZLIB_LIBRARIES}
 ${CMAKE_THREAD_LIBS_INIT} 
 ${GTK2_LIBRARIES}
 ${NAV2API_LIBS} 
 ${APPEXAMPLE_LIBS} 
 ${MAPLIBNG_LIBS} 
 ${PALLIB_LIBS} 
 ${GUBI_LIBS} 
 ${NAV2LIB_LIBS} 
 ${BTMAN_LIBS} 
 ${FAKENAVSRVCOMMOD_LIBS} 
 ${QSMOD_LIBS} 
 ${COMBSEARCH_LIBS} 
 ${GPSSIRFMOD_LIBS} 
 ${NAVSRVCOMMOD_LIBS} 
 ${COMMONMOD_LIBS} 
 ${UICTRLMOD_LIBS} 
 ${NAVTASK_LIBS} 
 ${GUIPROT_LIBS} 
 ${TCPSERIALMOD_LIBS}
 ${CTRLHUBMOD_LIBS} 
 ${SHARED_LIBS} 
 ${DESTMOD_LIBS} 
 ${PARAMMOD_LIBS} 
 ${COMMONLIB_LIBS} 
 ${OPENGL_LIBS}
 "/lib/libgthread-2.0.so"
)
ENDIF(NOT XCODE)
