# Create a library called "PAL"
include_directories(
  include/
  Linux/include/
  ../CommonLib/include/
  ../CommonLib/POSIX/include/
  ../CommonLib/POSIX/include/JTC/
)

AUX_SOURCE_DIRECTORY(src/ CUR_DIR_SOURCEFILES)
IF(UNIX AND NOT XCODE)
  SET(EXTRASOURCE Linux/src/PALNetwork.cpp Linux/src/PALThreading.cpp Linux/src/PALRunLoop.cpp)
ENDIF(UNIX AND NOT XCODE)
IF(XCODE)
  SET(EXTRASOURCE iPhone/src/PALGraphics.cpp iPhone/src/PALRunLoop.mm iPhone/src/PALThreading.mm)
ENDIF(XCODE)

IF(OPENGL_PAL_TEXT_RENDERING)
  SET(PALSOURCE Linux/src/PALGraphics.cpp Linux/src/PALFont.c Linux/src/outline.c Linux/harfbuzz/src/harfbuzz-shaper-all.cpp Linux/harfbuzz/contrib/harfbuzz-unicode.c Linux/harfbuzz/contrib/harfbuzz-unicode-tables.c Linux/harfbuzz/contrib/harfbuzz-freetype.c Linux/harfbuzz/src/harfbuzz.c Linux/harfbuzz/src/harfbuzz-impl.c Linux/harfbuzz/src/harfbuzz-stream.c)
  INCLUDE_DIRECTORIES(${FREETYPE_INCLUDE_DIRS})
  INCLUDE_DIRECTORIES(Linux/harfbuzz/src/)
  INCLUDE_DIRECTORIES(Linux/harfbuzz/contrib/)
ELSE(OPENGL_PAL_TEXT_RENDERING)
  IF(NOT XCODE)
    SET(PALSOURCE Linux/src/PALGraphicsGtk.cpp)
  ENDIF(NOT XCODE)
ENDIF(OPENGL_PAL_TEXT_RENDERING)

add_library(PAL ${PALSOURCE} ${CUR_DIR_SOURCEFILES} ${EXTRASOURCE})
