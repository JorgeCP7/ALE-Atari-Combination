MODULE := src/statistics

MODULE_OBJS := \
	src/statistics/Statistics.o \

MODULE_DIRS += \
	src/statistics

# Include common rules 
include $(srcdir)/common.rules
