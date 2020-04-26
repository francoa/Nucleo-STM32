# Mbed

- Go to [ARM Mbed web page](https://os.mbed.com/) and sign up
- Select your [board](https://os.mbed.com/platforms/ST-Nucleo-F746ZG/)
- Since I work in Linux, I selected Mbed CLI. Follow instructions [here](https://os.mbed.com/docs/mbed-os/v5.13/tools/manual-installation.html)
- Download and install a compiler. There are several options. I selected GNU Arm Embedded toolchain version 8-2019-q3-update.
-- Don't forget to run `mbed config -G GCC_ARM_PATH "/home/Dev_Env/gcc-arm-none-eabi-8-2019-q3-update/bin"` (if you've placed the compiler there, always use full path).
- Cd to the MBED_OS folder (current folder) and run:
	 `cd $MBED_OS
	 mbed import mbed-os
	 mbed config -G MBED_OS_DIR $MBED_OS/mbed-os
	 cd mbed-os
	 pip install -r requirements.txt`

# Mbed OS project

Composed of:
- Target, defined in mbed-os/targets/targets.json and optionally in custom_targets.json. Our target will be NUCLEO_F756ZG. 
- Configuration, optionally defined in mbed_lib.json and mbed_app.json. These define the settings for our application via macros 
- Toolchain and build profile, which define the arguments to the compiler suite
- Resources such as C sources, C++ sources, and configuration files

# Installation steps

- sudo aptitude install python git mercurial python-pip
- pip install mbed-cli

If you get Permission denied errors with pip installations, run pip with `--user` option. DO NOT USE sudo.

# If you need to reinstall

- `pip list | grep mbed` and then run `pip uninstall {elements in list}`
