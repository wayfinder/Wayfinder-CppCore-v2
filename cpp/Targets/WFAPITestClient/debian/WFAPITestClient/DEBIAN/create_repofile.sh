#!/bin/sh
cat debian/WFAPITestClient/DEBIAN/control.template | sed -e "s/#ver#/$2/g"
echo Filename: pool/diablo/non-free/$1
echo Size: `ls -la $1 | cut -d " " -f 6`
echo MD5sum: `md5sum $1 | cut -d " " -f 1`
