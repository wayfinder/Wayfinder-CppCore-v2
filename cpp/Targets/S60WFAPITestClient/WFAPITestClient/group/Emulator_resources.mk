# ============================================================================
#  Name	 : Icons_aif_scalable_dc.mk
#  Part of  : WFAPITestClient
#
#  Description:
# 
# ============================================================================


ifeq (WINS,$(findstring WINS, $(PLATFORM)))
TARGET_DIR=$(EPOCROOT)epoc32\winscw\c\shared\WFAPITestClient
#TARGET_DIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\Z
else
TARGET_DIR=$(EPOCROOT)epoc32\data\z
endif

# ----------------------------------------------------------------------------
# TODO: Configure these
# ----------------------------------------------------------------------------

NAV2DATA=..\nav2data
SVGDATA=..\gfx\overlay


$(TARGET_DIR)\params:
	@mkdir $(TARGET_DIR)\params

$(TARGET_DIR)\overlay:
	@mkdir $(TARGET_DIR)\overlay




copy_resources: $(TARGET_DIR)\params $(TARGET_DIR)\overlay
	mifconv $(SVGDATA)\overlay-item.mif /16,8 $(SVGDATA)\overlay-item.svg 
	mifconv $(SVGDATA)\overlay-item-bg.mif /16,8 $(SVGDATA)\overlay-item-bg.svg
	mifconv $(SVGDATA)\overlay-item-bg-stacked.mif /16,8 $(SVGDATA)\overlay-item-bg-stacked.svg
	mifconv $(SVGDATA)\overlay-item-bg-highlighted-stacked.mif /16,8 $(SVGDATA)\overlay-item-bg-highlighted-stacked.svg
	mifconv $(SVGDATA)\overlay-item-bg-highlighted.mif /16,8 $(SVGDATA)\overlay-item-bg-highlighted.svg
	mifconv $(SVGDATA)\dialog-background-3.mif /16,8 $(SVGDATA)\dialog-background-3.svg
	mifconv $(SVGDATA)\dialog-background-2.mif /16,8 $(SVGDATA)\dialog-background-2.svg
	mifconv $(SVGDATA)\dialog-arrow-left.mif /16,8 $(SVGDATA)\dialog-arrow-left.svg
	mifconv $(SVGDATA)\dialog-arrow-right.mif /16,8 $(SVGDATA)\dialog-arrow-right.svg
	mifconv $(SVGDATA)\dialog-frame.mif /16,8 $(SVGDATA)\dialog-frame.svg

	copy $(NAV2DATA)\paramseed.txt $(TARGET_DIR)\params\paramseed.txt
	copy $(SVGDATA)\overlay-item.mif $(TARGET_DIR)\overlay\overlay-item.mif
	copy $(SVGDATA)\overlay-item-bg.mif $(TARGET_DIR)\overlay\overlay-item-bg.mif
	copy $(SVGDATA)\overlay-item-bg-stacked.mif $(TARGET_DIR)\overlay\overlay-item-bg-stacked.mif
	copy $(SVGDATA)\overlay-item-bg-highlighted-stacked.mif $(TARGET_DIR)\overlay\overlay-item-bg-highlighted-stacked.mif
	copy $(SVGDATA)\overlay-item-bg-highlighted.mif $(TARGET_DIR)\overlay\overlay-item-bg-highlighted.mif

	copy $(SVGDATA)\dialog-background-3.mif $(TARGET_DIR)\overlay\dialog-background-3.mif
	copy $(SVGDATA)\dialog-background-2.mif $(TARGET_DIR)\overlay\dialog-background-2.mif
	copy $(SVGDATA)\dialog-arrow-left.mif $(TARGET_DIR)\overlay\dialog-arrow-left.mif
	copy $(SVGDATA)\dialog-arrow-right.mif $(TARGET_DIR)\overlay\dialog-arrow-right.mif
	copy $(SVGDATA)\dialog-frame.mif $(TARGET_DIR)\overlay\dialog-frame.mif
	echo Resources initialized.

.PHONY: copy_resources

RESOURCE : copy_resources

# Miscellaneous garbage

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(PARAMTARGETFILENAME)

FINAL : do_nothing

