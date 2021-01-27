from os.path import join
from os.path import expandvars
Import("env", "projenv")

env.AddPostAction(
	join("$BUILD_DIR","${PROGNAME}.elf"),
	env.VerboseAction(" ".join([
		"$OBJCOPY", "-O ihex", "$TARGET", # TARGET=.pio/build/fysetc_STM32F1/firmware.elf
		"\"" + join("$BUILD_DIR","${PROGNAME}.hex") + "\"", # Note: $BUILD_DIR is a full path
	]), "Building $TARGET"))
