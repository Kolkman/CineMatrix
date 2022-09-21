![GitHub release (release name instead of tag name)](https://img.shields.io/github/v/release/kolkman/cinematrix?include_prereleases)
![GitHub issues](https://img.shields.io/github/issues/kolkman/cinematrix)

# CineMatrix Display Web Server
<center>
<img src="WEBSources/MatrixDisplay.png" alt="Desktop Screeenshot" width=65% />
</center>


## What is it
A matrix display driver that is configurable through a web interface.

## How to use
Connect your ESP-32 Dev Kit C to your MAX72XX Matrix array: the following defaults: 
DATA_PIN 25, CS_PIN 26, CLK_PIN 27. Make sure tho select the 5V terminal on the ESP32 
and connect ground to a ground terminal.

After powering up the ESP32 it will set up a wireless access point. The matrix display 
will show its name and password on first start up and print some instructions on how to 
connect your browser.

## Installing
The distributed binaries are build for an ESP32 dev board with a display of twelve 8x8 matrix 
elements of hardware type [MD_MAX72XX::FC16_HW](https://majicdesigns.github.io/MD_MAX72XX/page_new_hardware.html).

If you have have the same hardware then you can follow this installation procedure.

Pick the latest release from [GitHub](https://github.com/Kolkman/CineMatrix/releases) and install 
using the [Espressif esptool.py](https://docs.espressif.com/projects/esptool/en/latest/esp32/) script as follows:

```
 
    tar -xvzf /path/to/release.tar 
    esptool.py --chip esp32 --port "/dev/tty.usbserial-0001" --baud 460800 --before default_reset \
   --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m  --flash_size 4MB \
     0x1000 bootloader_dio_40m.bin 0x8000 partitions.bin 0xe000 boot_app0.bin 0x10000 firmware.bin
```

Note that on Mac OS X you might experience that the serial to USB interface does not work. 
In those cases [these drivers](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers)
may offer a solution.


## Building

 If you have another board or a different size or type of display you have to build the code from source
 (using [PlatformIO](https://platformio.org/) in Visual Studio (preferably with MinifyAll extentension enabled).

## Security

The security is based on the ability to connect to the access point and thus on wireless security protocols.

## Acknowledgements

This code was developed for [Filmtheater Gerrit](http://filmtheater-gerrit.nl) on Visual Studio Code
for Mac OS X with PlatformIO. It relies heavily on [the Parola for Arduino Library](https://majicdesigns.github.io/MD_Parola/index.html) 
and borrows code fragment from [ESP32ressoMachine](https://kolkman.github.io/ESP32ressoMachine/), one of my other projects.





## Screenshots

<img src="WEBSources/Screenshot-desktop.png" alt="Desktop Screeenshot" width=65% />
<img src="WEBSources/Screenshot-iphone.png" alt="iPhone Screenshot" width=30% />




