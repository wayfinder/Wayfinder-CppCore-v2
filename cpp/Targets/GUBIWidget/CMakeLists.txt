# Create a library called "GUBIWidget"

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
    ../GUBI/include/
)

IF(UNIX)
  AUX_SOURCE_DIRECTORY(src/ CUR_DIR_SOURCEFILES)

  IF(NOT XCODE)
    include_directories(
      Gtk/include/
      ../MapLibNG/Export/include/Unix/
      ../MapLibNG/Unix/include/
      ../../arch-include/i386-redhat-linux/
    )
    IF(OPENGL_CLIENT)
      set (OPENGL_LIBS -lEGL -lGLES_CM)
    ENDIF(OPENGL_CLIENT)
    AUX_SOURCE_DIRECTORY(Gtk/src/Window/ CUR_DIR_SOURCEFILES)
  ENDIF(NOT XCODE)
ENDIF(UNIX)

add_library(GUBIWidget ${CUR_DIR_SOURCEFILES})
