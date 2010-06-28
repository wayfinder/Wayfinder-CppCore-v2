#sgit clone git://anongit.freedesktop.org/harfbuzz ../harfbuzz || (echo "Could not git clone from freedesktop.org, bailing out!" && exit 1)
git clone git://anongit.freedesktop.org/harfbuzz ../harfbuzz
ret=$?
#ret == 1 means ../harfbuzz already exists
if [ "$ret" != "0" ] && [ "$ret" != "1" ]; then
   echo "Could not git clone from freedesktop.org, bailing out!"
   exit 1
fi


mkdir -p cpp/Targets/PAL/Linux/harfbuzz/src/
mkdir -p cpp/Targets/PAL/Linux/harfbuzz/contrib/tables

cd ../harfbuzz/contrib/tables/
wget -q http://www.unicode.org/Public/5.1.0/ucd/extracted/DerivedGeneralCategory.txt
wget -q http://www.unicode.org/Public/5.1.0/ucd/extracted/DerivedCombiningClass.txt
wget -q http://www.unicode.org/Public/UNIDATA/auxiliary/GraphemeBreakProperty.txt
wget -q http://www.unicode.org/Public/5.1.0/ucd/Scripts.txt
wget -q http://skia.googlecode.com/svn-history/r536/trunk/third_party/harfbuzz/contrib/tables/mirroring-properties.h
python category-parse.py DerivedGeneralCategory.txt category-properties.h
python combining-class-parse.py DerivedCombiningClass.txt combining-properties.h
python grapheme-break-parse.py GraphemeBreakProperty.txt grapheme-break-properties.h
python scripts-parse.py Scripts.txt script-properties.h
cd - > /dev/null

cp ../harfbuzz/src/harfbuzz-shaper-all.cpp cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/contrib/harfbuzz-unicode.c cpp/Targets/PAL/Linux/harfbuzz/contrib/
cp ../harfbuzz/contrib/harfbuzz-unicode-tables.c cpp/Targets/PAL/Linux/harfbuzz/contrib/
cp ../harfbuzz/contrib/harfbuzz-freetype.c cpp/Targets/PAL/Linux/harfbuzz/contrib/
cp ../harfbuzz/src/harfbuzz.c cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-impl.c cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-stream.c cpp/Targets/PAL/Linux/harfbuzz/src/

cp ../harfbuzz/src/harfbuzz-shaper.h cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-global.h cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-gpos.h cpp/Targets/PAL/Linux/harfbuzz/src/

cp ../harfbuzz/src/harfbuzz-gdef.h cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-buffer.h cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-gsub.h cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-external.h cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-stream-private.h cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/contrib/harfbuzz-unicode.h cpp/Targets/PAL/Linux/harfbuzz/contrib/
cp ../harfbuzz/contrib/harfbuzz-freetype.h cpp/Targets/PAL/Linux/harfbuzz/contrib/
cp ../harfbuzz/src/harfbuzz-open.h cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-stream.h cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-impl.h cpp/Targets/PAL/Linux/harfbuzz/src/

cp ../harfbuzz/src/harfbuzz-shaper.cpp cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-indic.cpp cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-greek.c cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-tibetan.c cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-khmer.c cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-hebrew.c cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-arabic.c cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-hangul.c cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-myanmar.c cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-thai.c cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-shaper-private.h cpp/Targets/PAL/Linux/harfbuzz/src/

cp ../harfbuzz/contrib/tables/grapheme-break-properties.h cpp/Targets/PAL/Linux/harfbuzz/contrib/tables
cp ../harfbuzz/contrib/tables/mirroring-properties.h cpp/Targets/PAL/Linux/harfbuzz/contrib/tables
cp ../harfbuzz/contrib/tables/script-properties.h cpp/Targets/PAL/Linux/harfbuzz/contrib/tables
cp ../harfbuzz/contrib/tables/category-properties.h cpp/Targets/PAL/Linux/harfbuzz/contrib/tables
cp ../harfbuzz/contrib/tables/combining-properties.h cpp/Targets/PAL/Linux/harfbuzz/contrib/tables

cp ../harfbuzz/src/harfbuzz-buffer.c cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-gdef.c cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-gsub.c cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-gpos.c cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-open.c cpp/Targets/PAL/Linux/harfbuzz/src/

cp ../harfbuzz/src/harfbuzz-gdef-private.h cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-open-private.h cpp/Targets/PAL/Linux/harfbuzz/src/

cp ../harfbuzz/src/harfbuzz-buffer-private.h cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-gsub-private.h cpp/Targets/PAL/Linux/harfbuzz/src/
cp ../harfbuzz/src/harfbuzz-gpos-private.h cpp/Targets/PAL/Linux/harfbuzz/src/

echo All done.
