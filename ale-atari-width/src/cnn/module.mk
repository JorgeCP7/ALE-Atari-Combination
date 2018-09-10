MODULE := src/cnn

MODULE_OBJS := \
	src/cnn/CNNController.o \

MODULE_DIRS += \
	src/cnn

# Include common rules 
include $(srcdir)/common.rules
