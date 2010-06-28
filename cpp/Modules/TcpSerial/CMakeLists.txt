# Create a library called "TcpSerialModule"
include_directories(
  ../include/
  ../../include/
  ../../arch-include/i386-redhat-linux/
  ../../Targets/CommonLib/include/
  ../../Targets/CommonLib/POSIX/include/
  )

AUX_SOURCE_DIRECTORY(. CUR_DIR_SOURCEFILES)
add_library(TcpSerialModule ${CUR_DIR_SOURCEFILES})
