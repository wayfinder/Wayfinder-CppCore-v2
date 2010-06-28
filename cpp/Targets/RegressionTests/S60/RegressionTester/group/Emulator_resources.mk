# ============================================================================
#  Name	 : Emulator_resources.mk
#  Part of  : RegressionTester
#
#  Description:
# 
# ============================================================================


ifeq (WINS,$(findstring WINS, $(PLATFORM)))
TARGET_DIR=$(EPOCROOT)epoc32\winscw\c\shared\RegressionTester\nav2data
#TARGET_DIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\Z
else
TARGET_DIR=$(EPOCROOT)epoc32\data\z
endif

# ----------------------------------------------------------------------------
# TODO: Configure these
# ----------------------------------------------------------------------------

NAV2DATA=..\nav2data

$(TARGET_DIR)\params:
	@mkdir $(TARGET_DIR)\params

copy_resources: $(TARGET_DIR)\params 
	copy $(NAV2DATA)\paramseed.txt $(TARGET_DIR)\params\paramseed.txt
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

