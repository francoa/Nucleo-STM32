# Building our first MBED_OS program

First, let's create the folder for our program

`cd $MBED_OS
mbed new mbed-first-program`

We'll have the following directory structure then:

$MBED_OS
|- mbed-first-program
|- mbed-os

Let's configure our default target and toolchain for program `mbed-first-program`:

`cd mbed-first-program
mbed target NUCLEO_F756ZG
mbed toolchain GCC_ARM`

Running `mbed config -L` will show us that the configuration was properly done. Also, if the board is connected, `mbed detect` will show whether the toolchain we selected is supported or not.

Now we can run commands like `mbed compile --config`, which will show us the macros defined in `mbed_lib.json` and `mbed_app.json`.

For our first program, let's just echo the input of the serial port. We need main.cpp for this.

I've been having a problem though. The mbed commands show the following `You can install all missing modules by running "pip install -r requirements.txt" in "Nucleo-STM32/MBED_OS/mbed-os" folder`. Running that command in `$MBED_OS/mbed-os` creates a.... segmentation fault.

Let's analyze it the core dump and the memory. To analyze the core dump, we must first generate it by running `ulimit -c unlimited` and then `sysctl -w kernel.core_pattern=/tmp/crash/core.%u.%e.%p`. Reproduce the issue and run `gdb -c /tmp/core.1000.pip.19103`.

At the end, the only thing I needed to do was to upgrade pip by running `pip install -U pip` and then closing and reopening the terminal.

However, I'm still getting the following `The Mbed OS tools in this program require the following Python modules: hidapi, pywin32, wmi` and the requirments.txt stuff. Well, it seems mbed 1.10.1 resolves this. Go to `$MBED_OS/mbed-os` and run `mbed update master` and then `pip install -U mbed-cli`. If you get an error, then reinstall. I needed to reinstall, it was easier. You can see how in the README file of the MBED_OS folder.

Now you should have everything going. Let's compile our main.cpp. Run `mbed compile --flash` with the board connected to the USB port. I get an error: No Linker Script found. There's also a compiler version mismatch. The compiler version mismatch is an issue with MBED and shouldn't be worrysome as long as our compilation succeeds.

The linker seems to be at `$MBED_OS/mbed-os/targets/TARGET_STM/TARGET_STM32F7/TARGET_STM32F756xG/device/TOOLCHAIN_GCC_ARM`. I believe this problem could happen because we didn't import mbed-os library to our project. So run `mbed add mbed-os`. Compilation now starts. Run `mbed compile --flash`.

Install minicom to connect to the serial port. Run `mbed detect` so you can now which port is your board using. Configure Baud as 9600 and hardware control flow as NO. There, you have your first program running.