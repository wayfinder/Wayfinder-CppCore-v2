# Create a library called "Shared"

include_directories(
  ../include/
  ../../Resources/Loc/
  ../Targets/CommonLib/include/
  )

IF(UNIX)
  include_directories(
    ../arch-include/i386-redhat-linux/
    ../Targets/CommonLib/POSIX/include/
    )
ENDIF(UNIX)

add_subdirectory (GuiProt)
add_subdirectory (CombinedSearch)

AUX_SOURCE_DIRECTORY(. CUR_DIR_SOURCEFILES)

IF(UNIX)
AUX_SOURCE_DIRECTORY(i386-redhat-linux/ CUR_DIR_SOURCEFILES)
ENDIF(UNIX)

add_library(Shared ${CUR_DIR_SOURCEFILES})
