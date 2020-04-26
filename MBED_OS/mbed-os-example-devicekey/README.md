# Using DeviceKey

Seems like we need some config to do. The compiler is not properly including the DeviceKey.h file.

I needed to add `"target.components_add" : ["FLASHIAP"]` to mbed_app.json. There can also be some "config" tag at the same level as "target_overrides"

# Next steps

- Device Key
- Smart Door Lock
- Secure Boot loader

# Running tests

`mbedhtrun -d . -p /dev/ttyACM0 -m NUCLEO_F756ZG -f BUILD/NUCLEO_F756ZG/GCC_ARM/mbed-os-example-devicekey.bin --compare-log tests/devicekey.log`

However, I'm getting a sync failed message.

And now I seem to have bricked the device.

Well, not bricked, but not working either. Let's try to debug.

We first need to start a debug server. To connect to the debug interface, OpenOCD support most boards. If the debug interface on your board is classified as 'CMSIS-DAP' or 'DAPLink' (most boards), you can use pyOCD. If not, use OpenOCD.

First, make sure you have installed the GNU Arm Embedded Toolchain and added it to your PATH. To verify that you have the correct version installed, open a terminal and run: `arm-none-eabi-gdb --version`. I do not have it in my path, I need to add it. 

Let's try with pyOCD, if we have our target: `pip install pyocd --user` then run `pyocd list --targets`. Look for your on-board debugger chip in the list. I don't have it. Let's use openOCD then. Install it with `apt-get`. Then run:
`openocd -f /usr/share/openocd/scripts/board/stm32f7discovery.cfg -f /usr/share/openocd/scripts/interface/stlink-v2-1.cfg -c init -c "reset init"`
I get open failed because a speed difference. I need to configure it maybe. Well, no. Apparently the access to ports is root user only. Is this adviced? Maybe we can change the udev rules, so as to not use sudo.

Let's see how to add udev rules. Run `dmesg` to see the information of the board (vendor, product id, etc).

[15572.608880] usb 1-1.2: new full-speed USB device number 7 using ehci-pci
[15572.719144] usb 1-1.2: New USB device found, idVendor=0483, idProduct=374b
[15572.719150] usb 1-1.2: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[15572.719153] usb 1-1.2: Product: STM32 STLink
[15572.719156] usb 1-1.2: Manufacturer: STMicroelectronics
[15572.774407] usb-storage 1-1.2:1.1: USB Mass Storage device detected
[15572.774736] scsi host4: usb-storage 1-1.2:1.1
[15572.775393] cdc_acm 1-1.2:1.2: ttyACM0: USB ACM device
[15573.777860] scsi 4:0:0:0: Direct-Access     MBED     microcontroller  1.0  PQ: 0 ANSI: 2

Adapt [this](https://github.com/mbedmicro/pyOCD/blob/master/udev/49-stlinkv2-1.rules) to your needs based on the above information. Then run `sudo udevadm control --reload` and `sudo udevadm trigger` to avoid having to reboot. This works.

Now we need debug build that contains .elf files `mbed compile --profile mbed-os/tools/profiles/debug.json --flash`. Note: Make sure to do a clean build when switching to and from debug and release by removing the BUILD folder.

Now, let's connect to the GDB server `arm-none-eabi-gdb path/to/elf/file`. Next, run in GDB `target remote localhost:3333`

That's cool, but still the example is failing. Might it be the storage configuration? I removed all, and still failing. Let's try by creating a new project altogether.

- It's not the requirements.txt
OK, it seems that it was the version of mbed-os library added to the projects

If ever out of space, disconnect the device.

I expect that with PSA, I will be able to show what happens when running code is not properly isolated, by using a remote shell attack.

I did this run by removing the configuration in mbed_app.json that indicated the location of the KVStore. I left it with default settings. Let's now attempt the testing. Close minicom before doing so. SUCCESS!





DeviceKey
DeviceKey is a mechanism that implements key derivation from a root of trust key. The DeviceKey mechanism generates symmetric keys that security features need. You can use these keys for encryption, authentication and more. The DeviceKey API allows key derivation without exposing the actual root of trust, to reduce the possibility of accidental exposure of the root of trust outside the device.

We have implemented DeviceKey according to NIST SP 800-108, section "KDF in Counter Mode", with AES-CMAC as the pseudorandom function.

Root of Trust
The root of trust key, which DeviceKey uses to derive additional keys, is generated using the hardware random generator if it exists, or using a key injected to the device in the production process.

The characteristics required by this root of trust are:

It must be unique per device.
It must be difficult to guess.
It must be at least 128 bits.
It must be kept secret.
The DeviceKey feature keeps the root of trust key in internal storage, using the NVStore component. Internal storage provides protection from external physical attacks to the device.

The root of trust is generated at the first use of DeviceKey if the true random number generator is available in the device. If no true random number generator is available, you must pass the injected root of trust key to the DeviceKey before you call the key derivation API.

Key derivation API
generate_derived_key: This API generates a new key based on a string (salt) the caller provides. The same key is generated for the same salt. Generated keys can be 128 or 256 bits in length.

Root of Trust Injection API
device_inject_root_of_trust: You must call this API once in the lifecycle of the device, before any call to key derivation, if the device does not support True Random Number Generator (DEVICE_TRNG is not defined).