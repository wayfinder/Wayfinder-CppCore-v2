# Create a library called "NavTask"

include_directories(.
  ../include/
  ../../include/
  ../../Targets/CommonLib/include/
  )

IF(UNIX)
  include_directories(
    ../../arch-include/i386-redhat-linux/
    ../../Targets/CommonLib/POSIX/include/
    )
ENDIF(UNIX)



AUX_SOURCE_DIRECTORY(. CUR_DIR_SOURCEFILES)
IF(UNIX)
  AUX_SOURCE_DIRECTORY(i386-redhat-linux/ CUR_DIR_SOURCEFILES)
ENDIF(UNIX)

add_library(NavTask ${CUR_DIR_SOURCEFILES})
