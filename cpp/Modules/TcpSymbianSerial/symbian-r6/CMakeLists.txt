# Create a library called "TcpSymbianSerialModule"

include_directories(../include/
  ../../include/
  ../../arch-include/symbian-r6/
  ../../Targets/CommonLib/include/
  ../../Targets/CommonLib/S60/include/
  ../../Targets/CommonLib/Export/include/
)


AUX_SOURCE_DIRECTORY(. CUR_DIR_SOURCEFILES)
add_library(TcpSymbianSerialModule ${CUR_DIR_SOURCEFILES})
