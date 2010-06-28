# Create a library called "AppExamples"

include_directories(
  Shared/include/
  ../MapLibNG/Shared/include/
  ../CommonLib/include/
  ../MapLibNG/Export/include/Shared/
  ../MapLibNG/Export/internal/include/
  ../PAL/include/
  ../RegressionTests/Export/include/
  ../RegressionTests/Export/include/Shared/
  ../GUBI/include/
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
ENDIF(UNIX)

IF(XCODE)
  include_directories(
    iPhone/include/
    ../MapLibNG/iPhone/include/
    ../MapLibNG/Unix/include/no_nav2/
  )
  AUX_SOURCE_DIRECTORY(iPhone/src/Window/ CUR_DIR_SOURCEFILES)
ENDIF(XCODE)

AUX_SOURCE_DIRECTORY(Shared/src/Window/ CUR_DIR_SOURCEFILES)
AUX_SOURCE_DIRECTORY(Shared/src/Control/ CUR_DIR_SOURCEFILES)
AUX_SOURCE_DIRECTORY(Shared/src/Control/Input/ CUR_DIR_SOURCEFILES)

add_library(AppExamples ${CUR_DIR_SOURCEFILES})
