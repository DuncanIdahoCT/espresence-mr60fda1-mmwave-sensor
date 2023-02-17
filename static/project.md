## The build

![Enclosure Case](images/enclosure%20case.png)

#### I reused a simple case that I had already and as chance would have it, the radar module fit inside it, barely but it fits! I chose an ESP MCU from Adafruit that's tiny so that it would nicely tuck in behind the radar module and also because the plug makes this a bit of a bulky, not easy to stack, build... and using an ESP8266 module would have made it difficult to keep the build small

![Radar Module Connections](images/radar%20module%20connections.png)

![MCU Module Connections](images/mcu%20module%20connections.png)

#### For now I used the small bit of foam that was shipped with the radar module pressed into it to isolate the two modules and help prevent any contact that would short things out

![Radar MCU Sandwitch](images/radar%20mcu%20sandwitch.png)

#### Initially, I had some fun getting the Adafruit ESP modules to play nice with ESPHome local webtools that are used to install and update the MCU. I don't recall the exact issue but this board has a specific boot mode to allow install over USB and then a normal boot up to run the installed image. I found a reference to this platformio config entry that seems to keep ESPHome happy and allows normal management and installation of OTA updates. However, it does also seem to prevent the USB connect option of ESPHome, and the unit doesn't present as a USB com port at all until you hold the boot button during power up.

```
  platformio_options:
    board_build.extra_flags:
      - "-DARDUINO_USB_CDC_ON_BOOT=0"  # Override, defaults to '-DARDUINO_USB_CDC_ON_BOOT=1'
```
