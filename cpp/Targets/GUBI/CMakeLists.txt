# Create a library called "GUBI"

include_directories(
  Shared/include/
  ../MapLibNG/Shared/include/
  ../CommonLib/include/
  ../MapLibNG/Export/include/Shared/
  ../MapLibNG/Export/internal/include/
  ../PAL/include/
  ../RegressionTests/Export/include/
  ../RegressionTests/Export/include/Shared/
  include/
  ../FMath/include/
)

IF(UNIX)
  IF(NOT XCODE)
    include_directories(
      Gtk/include/
      ../MapLibNG/Export/include/Unix/
      ../MapLibNG/Unix/include/
      ../../arch-include/i386-redhat-linux/
    )
    AUX_SOURCE_DIRECTORY(Gtk/src/Window/ CUR_DIR_SOURCEFILES)
  ENDIF(NOT XCODE)
  IF(OPENGL_CLIENT OR XCODE)
    set (OPENGL_LIBS -lEGL -lGLES_CM)
    AUX_SOURCE_DIRECTORY(src/Renderer/ CUR_DIR_SOURCEFILES)
    AUX_SOURCE_DIRECTORY(src/OpenGLES/ CUR_DIR_SOURCEFILES)
    AUX_SOURCE_DIRECTORY(src/Core/ CUR_DIR_SOURCEFILES)
    AUX_SOURCE_DIRECTORY(src/Property/ CUR_DIR_SOURCEFILES)
    AUX_SOURCE_DIRECTORY(src/Input/ CUR_DIR_SOURCEFILES)
    AUX_SOURCE_DIRECTORY(src/Effects/ CUR_DIR_SOURCEFILES)
    AUX_SOURCE_DIRECTORY(src/Widget/ CUR_DIR_SOURCEFILES)
    AUX_SOURCE_DIRECTORY(src/Signal/ CUR_DIR_SOURCEFILES)
    AUX_SOURCE_DIRECTORY(src/View/ CUR_DIR_SOURCEFILES)
  ENDIF(OPENGL_CLIENT OR XCODE)
ENDIF(UNIX)

IF(XCODE)
  include_directories(
    iPhone/include/
  )
  AUX_SOURCE_DIRECTORY(iPhone/src/Window/ CUR_DIR_SOURCEFILES)
ENDIF(XCODE)

AUX_SOURCE_DIRECTORY(src/Common/ CUR_DIR_SOURCEFILES)

add_library(GUBI ${CUR_DIR_SOURCEFILES})
