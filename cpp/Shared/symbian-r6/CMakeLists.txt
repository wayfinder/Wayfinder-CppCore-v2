# Create a library called "symbian -r6"
include_directories(.
  ../../include/
)

AUX_SOURCE_DIRECTORY(. CUR_DIR_SOURCEFILES)
add_library(symbian-r6 ${CUR_DIR_SOURCEFILES})
