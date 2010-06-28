# Create a library called "CommonModule"

include_directories(
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
add_library(CommonModule ${CUR_DIR_SOURCEFILES})

