# Create a library called "CombinedSearch"

include_directories(
  ../../include/CombinedSearch/
  ../../include/CombinedSearch/NGP/
  ../../Modules/include/
  ../../Targets/PAL/include/
  )

AUX_SOURCE_DIRECTORY(. CUR_DIR_SOURCEFILES)
add_library(CombinedSearch ${CUR_DIR_SOURCEFILES})
