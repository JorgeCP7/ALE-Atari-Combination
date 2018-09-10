MODULE := src/video

MODULE_OBJS := \
	src/video/VideoController.o \

MODULE_DIRS += \
	src/video

# Include common rules 
include $(srcdir)/common.rules
