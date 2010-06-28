# Create a library called "QueueSerialModule"

include_directories(
  ../include/
  ../../include/
  ../../Targets/CommonLib/include/
)

#includes for Linux
IF(UNIX)
  include_directories(  
    ../../arch-include/i386-redhat-linux/
    ../../Targets/CommonLib/POSIX/include/
    )
ENDIF(UNIX)

AUX_SOURCE_DIRECTORY(. CUR_DIR_SOURCEFILES)
add_library(QueueSerialModule ${CUR_DIR_SOURCEFILES})
