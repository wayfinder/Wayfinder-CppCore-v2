# Create a library called "Nav2API"
include_directories(
  Shared/include/
  Export/include/Shared/
  ../Nav2lib/
  ../PAL/include/
  ../../include/
  ../../Modules/include/
  ../../Modules/UiCtrl/
  ../../include/CombinedSearch/NGP/
  ../../include/GuiProt/
  ../../include/CombinedSearch/
  ../../Targets/CommonLib/include/
  ../MapLib/Export/include/Shared/
  ../MapLib/Export/internal/include/
  ../MapLib/Shared/include/
  )

AUX_SOURCE_DIRECTORY("Shared/src" CUR_DIR_SOURCEFILES)

IF(UNIX)
  IF(NOT XCODE)
    include_directories(
      Unix/include/
      Export/include/Unix
      ../CommonLib/Linux/include/
      ../MapLib/Unix/include
      ../../arch-include/i386-redhat-linux/
      ../../Targets/CommonLib/POSIX/include/
    )
    AUX_SOURCE_DIRECTORY("Unix/src" CUR_DIR_SOURCEFILES)
  ELSE(NOT XCODE)
    include_directories(
      iPhone/include/
      Export/include/iPhone
      ../CommonLib/iPhone/include/
      ../CommonLib/POSIX/include/
      ../MapLib/iPhone/include
      ../../arch-include/i386-redhat-linux/
    )
    AUX_SOURCE_DIRECTORY("iPhone/src" CUR_DIR_SOURCEFILES)
  ENDIF(NOT XCODE)
ENDIF(UNIX)

add_library(Nav2API ${CUR_DIR_SOURCEFILES})

