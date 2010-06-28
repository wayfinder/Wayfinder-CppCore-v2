# Create a library called "Nav2Lib"

include_directories(
  .
  ../../include/
  ../../Modules/include/
  ../CommonLib/include/
  )

IF(UNIX)
  include_directories( 
    ../../include/i386-redhat-linux/
    ../../arch-include/i386-redhat-linux/
    ../CommonLib/POSIX/include/
    )
ENDIF(UNIX)


AUX_SOURCE_DIRECTORY(. CUR_DIR_SOURCEFILES)
IF(UNIX)
  IF(NOT XCODE)
    AUX_SOURCE_DIRECTORY(i386-redhat-linux/ CUR_DIR_SOURCEFILES)
  ENDIF(NOT XCODE)
ENDIF(UNIX)

add_library(Nav2lib ${CUR_DIR_SOURCEFILES})
