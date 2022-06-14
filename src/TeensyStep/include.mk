# Specify chip here
CPPFLAGS = 		\
-DSTM32F103xE	\
-DSTM32F1xx

# Add define to show usage of TeensyStep
CPPFLAGS += -DTeensy_Step

TEENSYDIR     := $(dir $(lastword $(MAKEFILE_LIST)))

INCDIRS += $(TEENSYDIR)src/				\
$(TEENSYDIR)src/timer/					\
$(TEENSYDIR)src/hal_port/				\
$(TEENSYDIR)src/timer/generic/			\
$(TEENSYDIR)src/timer/stm32/

SOURCES += $(TEENSYDIR)src/ErrorHandler.cpp 			\
$(TEENSYDIR)src/Stepper.cpp								\
$(TEENSYDIR)src/hal_port/Port.cpp						\
$(TEENSYDIR)src/hal_port/HardwareTimer.cpp				\
$(TEENSYDIR)src/hal_port/timer.c						\
$(TEENSYDIR)src/timer/stm32/TimerField.cpp				\
$(TEENSYDIR)src/timer/generic/TickTimer.cpp