# Create a library called "MapLib"

include_directories(
  Export/internal/include/
  Export/include/Shared/
  Shared/include/
  ../CommonLib/include/
)

IF(UNIX)
  IF(NOT XCODE)
    include_directories(
      Export/include/Unix/
      Unix/include/
      ../../arch-include/i386-redhat-linux/
      ../PAL/include/
      Gtk/include/
    )
    AUX_SOURCE_DIRECTORY(Unix/src/ CUR_DIR_SOURCEFILES)
    AUX_SOURCE_DIRECTORY(Gtk/src/ CUR_DIR_SOURCEFILES)
  ENDIF(NOT XCODE)
ENDIF(UNIX)

IF(XCODE)
  include_directories(
    Shared/include/OpenGlEs/
    Export/include/iPhone/
    iPhone/include/
    ../../arch-include/i386-redhat-linux/
    ../PAL/include/
  )
  AUX_SOURCE_DIRECTORY(iPhone/src/ CUR_DIR_SOURCEFILES)
ENDIF(XCODE)

AUX_SOURCE_DIRECTORY(Shared/src/ CUR_DIR_SOURCEFILES)
AUX_SOURCE_DIRECTORY(Shared/src/Graphics/ CUR_DIR_SOURCEFILES)
AUX_SOURCE_DIRECTORY(Shared/src/Grid/ CUR_DIR_SOURCEFILES)
AUX_SOURCE_DIRECTORY(Shared/src/TextPlacement/ CUR_DIR_SOURCEFILES)
AUX_SOURCE_DIRECTORY(Shared/src/Triangulation/ CUR_DIR_SOURCEFILES)
AUX_SOURCE_DIRECTORY(Shared/src/Util/ CUR_DIR_SOURCEFILES)

IF(OPENGL_CLIENT OR XCODE)
  AUX_SOURCE_DIRECTORY(Shared/src/OpenGlEs/ CUR_DIR_SOURCEFILES)

  IF(NOT XCODE)
    AUX_SOURCE_DIRECTORY(Shared/src/Graphics/Text/ CUR_DIR_SOURCEFILES)
    AUX_SOURCE_DIRECTORY(Gtk/src/OpenGlEs/ CUR_DIR_SOURCEFILES)
  ENDIF(NOT XCODE)

  include_directories(
    Gtk/include/OpenGlEs/
    Shared/include/OpenGlEs/
  )
ENDIF(OPENGL_CLIENT OR XCODE)

add_library(MapLib ${CUR_DIR_SOURCEFILES})
