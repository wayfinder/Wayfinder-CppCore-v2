# Create a library called "OggVorbisLib"

include_directories(symbian/oggvorbisfile/
  symbian/tremor/
)

AUX_SOURCE_DIRECTORY(symbian/oggvorbisfile/ CUR_DIR_SOURCEFILES)
AUX_SOURCE_DIRECTORY(symbian/tremor/ CUR_DIR_SOURCEFILES)
add_library(OggVorbisLib ${CUR_DIR_SOURCEFILES})
