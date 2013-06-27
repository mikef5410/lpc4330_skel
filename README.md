lpc4330_skel
============

This is a minimal LPC4330 Cortex-M4 FreeRTOS skeleton. It runs an blinks a
couple of LEDs, one for each task. Compiles with Linaro arm-none-eabi-gcc +
GNUMake; nothing fancy, no stinking IDE, though I've included hooks to use it
with ECB on emacs.

Basics:

* You'll need an ARM (Cortex-M4) cross compiler. Linaro's works as well as any.
* You need basic development tools like GNUMake and emacs
* Clone the repo and run bootstrap. It'll pull in two submodules:
   1. FreeRTOS 
   2. lpcopen libraries
* There's nothing special about either at the moment. The boostrap script
  will copy the necessary bits of FreeRTOS to a new minimal tree called FreeRTOS from the external/FreeRTOS tree.
* Finally cd to application/ and run 'make'

The board I made has a builtin Opendous FT2232H-based JTAG interface. If you've got one of those, then 'make download' will work pretty much as-is.
