# Create a library called "CommonLib"
include_directories(
  include/
  ../../include/
  )

IF(UNIX)
  include_directories(
    Linux/include/
    POSIX/include/
    ../../arch-include/i386-redhat-linux/
    )
ENDIF(UNIX)

IF(XCODE)
  include_directories(
    iPhone/include/
    ../Nav2API/iPhone/include/
    ../Nav2API/Shared/include/
    ../Nav2API/Export/include/Shared/
    ../PAL/include/
    ../MapLib/Export/include/Shared/
  )
ENDIF(XCODE)

AUX_SOURCE_DIRECTORY(src/ CUR_DIR_SOURCEFILES)

IF(UNIX)
  AUX_SOURCE_DIRECTORY(Linux/src/ CUR_DIR_SOURCEFILES)
  AUX_SOURCE_DIRECTORY(POSIX/src/ CUR_DIR_SOURCEFILES)
  AUX_SOURCE_DIRECTORY(POSIX/src/JTC/ CUR_DIR_SOURCEFILES)
ENDIF(UNIX)

IF(XCODE)
  AUX_SOURCE_DIRECTORY(iPhone/src/ CUR_DIR_SOURCEFILES)
ENDIF(XCODE)

add_library(CommonLib ${CUR_DIR_SOURCEFILES})
