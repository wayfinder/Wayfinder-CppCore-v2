#!/bin/sh
OUT=WFAPITestClient
WWWHOST=www.wayfinder.com
VER=`date +%y%m%d`
ARCH=`arch`
BINSIZE=`du -k $(OUT) | cut -f 1`

mkdir -p debian/$(OUT)/usr/share/WFAPITestClient
mkdir -p debian/$(OUT)/usr/share/applications/hildon/
mkdir -p debian/$(OUT)/usr/bin/
mkdir -p debian/$(OUT)/usr/share/icons/26x26/hildon/
mkdir -p debian/$(OUT)/usr/share/icons/40x40/hildon/
mkdir -p debian/$(OUT)/usr/share/icons/scalable/hildon/
cp $(OUT) debian/WFAPITestClient/usr/bin
cp -r nav2data ../MapLib/Client/GtkDevClient/images debian/$(OUT)/usr/share/$(OUT)/
cp nav2data/paramseed.txt.dist debian/$(OUT)/usr/share/$(OUT)/nav2data/paramseed.txt
cp debian/$(OUT)/DEBIAN/maemo26.png debian/$(OUT)/usr/share/icons/26x26/hildon/$(OUT).png
cp debian/$(OUT)/DEBIAN/maemo40.png debian/$(OUT)/usr/share/icons/40x40/hildon/$(OUT)ppp.png
cp debian/$(OUT)/DEBIAN/maemo26.png debian/$(OUT)/usr/share/icons/scalable/hildon/$(OUT).png
strip debian/$(OUT)/usr/bin/$(OUT)
cat debian/$(OUT)/DEBIAN/control.template | sed -e 's/#ver#/$(VER)/g' | sed -e 's/#instsize#/$(BINSIZE)/g' > debian/$(OUT)/DEBIAN/control
cat debian/$(OUT)/DEBIAN/$(OUT).desktop.template | sed -e 's/#ver#/$(VER)/g' > debian/$(OUT)/usr/share/applications/hildon/$(OUT).desktop
dpkg -b debian/$(OUT) 2>&1 | grep -v " contains user-defined field" | grep -v "ignoring 1 warnings about the control file"
mv debian/$(OUT).deb debian/$(OUT)-$(VER)_$(ARCH).deb
sh debian/$(OUT)/DEBIAN/create_repofile.sh debian/$(OUT)-$(VER)_$(ARCH).deb $(VER) | gzip > Packages.gz
scp Packages.gz info.wayfinder.com:/var/www/html/maemo-repository/dists/diablo/non-free/binary-armel/
scp debian/$(OUT)-$(VER)_$(ARCH).deb $(WWWHOST):/var/www/html/maemo-repository/pool/diablo/non-free/debian/
rm -rf Packages.gz debian/$(OUT)-$(VER)_$(ARCH).deb debian/$(OUT)/usr debian/$(OUT)/DEBIAN/control
