IF(NOT XCODE)

# Create a library called "Nav2API"
SET(GTKMAPLIBSRCPATH "../MapLib/Client/GtkDevClient/src/")

# Set the image directory to MapLib
  SET(IMAGEDIR "../MapLib/Client/GtkDevClient/images")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIMAGE_DIR=\\\"${IMAGEDIR}\\\"")
# Set if ogg playback should be enabled
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHAVE_OGG123")
# Set if scratchbox is acting up
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-poison-system-directories")

include_directories(
  include
  ../CommonLib/Export/include/
  ../PAL/include
  ../Nav2API/Export/include/Shared
  ../Nav2API/Export/include/Unix
)

IF(LINKWITH_MAPLIBNG)
  include_directories(
    ../MapLibNG/Export/include/Shared
    ../MapLibNG/Export/include
    ../MapLibNG/Client/GtkDevClient/include
    ../MapLibNG/Export/internal/include
  )
  SET(MAPLIB_LIBS "MapLibNG")
ELSE(LINKWITH_MAPLIBNG)
  SET(MAPLIB_LIBS "MapLib")
  include_directories(
    ../MapLib/Export/include/Shared
    ../MapLib/Export/include
    ../MapLib/Client/GtkDevClient/include
    ../MapLib/Export/internal/include
  )
ENDIF(LINKWITH_MAPLIBNG)

AUX_SOURCE_DIRECTORY("src" CUR_DIR_SOURCEFILES)

SET(OTHER_SOURCEFILES ${GTKMAPLIBSRCPATH}GtkUtil.cpp ${GTKMAPLIBSRCPATH}/OverlayZoomSetup.cpp ${GTKMAPLIBSRCPATH}/GtkDevControl.cpp)

add_executable (WFAPITestClient ${OTHER_SOURCEFILES} ${CUR_DIR_SOURCEFILES})

SET(NAV2API_LIBS "Nav2API")
SET(NAV2LIB_LIBS "Nav2lib")
SET(COMMONLIB_LIBS "CommonLib")
SET(PALLIB_LIBS "PAL")
SET(DESTMOD_LIBS "DestinationsModule")
SET(UICTRLMOD_LIBS "UiCtrlModule")
SET(COMBSEARCH_LIBS "CombinedSearch")
SET(FAKENAVSRVCOMMOD_LIBS "FakeNavServerComModule")
SET(PARAMMOD_LIBS "ParameterModule")
SET(GPSSIRFMOD_LIBS "GpsSirfModule")
SET(QSMOD_LIBS "QueueSerialModule")
SET(COMMONMOD_LIBS "CommonModule")
SET(GUIPROT_LIBS "GuiProt")
SET(NAVSRVCOMMOD_LIBS "NavServerComModule")
SET(SHARED_LIBS "Shared")
SET(CTRLHUBMOD_LIBS "CtrlHubModule")
SET(NAVTASK_LIBS "NavTask")
SET(TCPSERIALMOD_LIBS "TcpSerialModule")

IF(OPENGL_CLIENT)
  SET(OPENGL_LIBS -lEGL -lGLES_CM)
ENDIF(OPENGL_CLIENT)

target_link_libraries(WFAPITestClient 
 ${ZLIB_LIBRARIES}
 ${CMAKE_THREAD_LIBS_INIT} 
 ${GTK2_LIBRARIES}
 ${NAV2API_LIBS} 
 ${PALLIB_LIBS} 
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
 ${i386_LIBS} 
 ${DESTMOD_LIBS} 
 ${PARAMMOD_LIBS} 
 ${COMMONLIB_LIBS}
 ${OPENGL_LIBS}
 "/lib/libgthread-2.0.so"
)

ENDIF(NOT XCODE)
