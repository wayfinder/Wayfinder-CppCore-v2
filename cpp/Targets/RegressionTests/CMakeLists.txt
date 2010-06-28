
IF(NOT XCODE)

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
  ../MapLib/Shared/include
  ../MapLib/Export/include/Shared
  ../MapLib/Export/internal/include
  ../Nav2API/Export/include/Unix
  ../MapLib/Export/include/Unix
  ../MapLib/Unix/include
  ../Nav2API/Unix/include
  ../../arch-include/i386-redhat-linux/
  ../../include
)

AUX_SOURCE_DIRECTORY(. CUR_DIR_SOURCEFILES)
AUX_SOURCE_DIRECTORY(src/ CUR_DIR_SOURCEFILES)
AUX_SOURCE_DIRECTORY(../CommonLib/POSIX/src/ CUR_DIR_SOURCEFILES)
AUX_SOURCE_DIRECTORY(../CommonLib/POSIX/src/JTC/Tests/ CUR_DIR_SOURCEFILES)


set (MAPLIB_LIBS "MapLib")
set (NAV2API_LIBS "Nav2API")
set (NAV2LIB_LIBS "Nav2lib")
set (COMMONLIB_LIBS "CommonLib")
set (PALLIB_LIBS "PAL")
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

add_executable (RegressionTester ${CUR_DIR_SOURCEFILES})

target_link_libraries(RegressionTester 
 ${ZLIB_LIBRARIES}
 ${CMAKE_THREAD_LIBS_INIT} 
 ${GTK2_LIBRARIES}
 ${NAV2API_LIBS} 
 ${MAPLIB_LIBS} 
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
 ${PALLIB_LIBS} 
 ${DESTMOD_LIBS} 
 ${PARAMMOD_LIBS} 
 ${COMMONLIB_LIBS} 
 "/lib/libgthread-2.0.so"
)

ENDIF(NOT XCODE)
