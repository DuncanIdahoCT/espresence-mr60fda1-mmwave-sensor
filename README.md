# espresence-mr60fda1-mmwave-sensor
Home Assistant / ESPHome Integration for the Seeed Studio 60GHz mmWave Fall Detection Pro Module

![ESPHome HA Integration](/static/images/HA%20Sensor.png)


![ESP Capive Portal](/static/images/ESP%20Captive%20Portal.png)

Some project notes and data on development testing can be found [here](/static/project.md)!

This is a fully functional ESP based integration with Home Assistant for the Seeed Studios 60GHz Fall Detection Pro radar module. I used an Adafruit QT PY ESP32-S2 for my setup but any ESP module that has a UART bus should work fine. The sensor does requrie 5V (not 3.3v) to function properly, or perhaps at all. I've managed to go through most of the currently in production (meaning not still underdevelopment) features listed on the Seeed protocol tables for this radar module and expose these options and sensors using ESPHome. I've personally tested the fall detection on max sensitivity and it "works" but YMMV for sure!

If you follow the Seeed forums or Discord, you'll see quite a few threads about how the fall detection is very spefic on placement, angle, FOV, range, etc...

The presence and motion range of the Fall Detection Pro module, even though it's only a firmware difference from the Resp/Heart module, is quite a bit higher at 6 Meters and 3 Meters for fall detection... my limited testing was well withing 3 meters.

# Sensors explained...

Not all the sensors require an introduction, presence, motion, heartrate... all seem pretty self explanetory but there are some caveats on how I chose to implement them in ESPHome and C++

   * Activity Level is a 0-100 scale representation of just how much movement the sensor is detecting, don't kill the messenger, I have no idea how the sensor determines this
   * Motion is an interesting one; I'm using it as a binary sensor, simply there is or is not motion. However, technically there are 3 states for this (none, stationary, and active) and I'm ignoring none because I can't think of why it’s even there given there is a separate dedicated presence function. Therefore, I'm mapping active and stationary to 1, and 0 respectively.
   * Presence is just a perfect binary status byte 1 or 0, present, not present.
   * Fall Detection is the secret sauce of this particular radar module, I can't say exactly how it works. My test was done at max sensitivity and all I did was lay down on the floor ¯\_(ツ)_/¯
   * Fall Detection Sensitivity is both a number sensor and a setting as is typical with these types of devices that send/receive settting using UART. I'm unsure what the default setting is, even resetting the radar module doesn't seem to change what you have set here.
   * Fall Detection Switch, self explanatory? :) ok fine... this simply enables or disables reporting of a detected fall or at least it's supposed to, I can tell it's taking effect because the radar reports it back but I didn't test it.

# Sensor implementation

You can see from the ESP captive portal image above that some of the sensors have "internal" template references that look like duplicates. These will only show up in the code or on the debug webtool for the ESP module. I used the publish to template method to be able to have binary and types using state data received to basic sensors from a single UART device component class. I think I've found a way to do this in a much cleaner way in the C++ / ESPHome YAML code but it's not on my top priority to-do list right now.

The presence sensor for example:
  ```
  - id: presence_sensor
    name: "internal_presense_sensor_to_binary_template"
    internal: true
    on_value:
      - binary_sensor.template.publish:
          id: presence_template
          state: !lambda return x > 0;
  ```
Has a corresponding binary_sensor reference:

  ```
  - platform: template
    id: presence_template
    name: "${room} Presence"
    device_class: occupancy
    filters:
      - delayed_off: 5s
  ```

The presence sensor is marked as internal: true and given a name to hint at this while the presence binary_sensor is the one that is presented to the integration and has the occupancy device class and proper states

There are likely other ways to get around the issues I was facing but I found this to be clean and easy enough and kept the include.h uart device component code nice and simple.

# Installation:
 * Download the C++ header file and copy it (keeping the subfolder paths) into your Home Assistant config/esphome main folder:

   ```
   header/esp-mmwave-60ghz-fda-sensor.h
   
   ```
 
 * In Home Assistant add-on, click ESPHome>open web gui and create a new device choosing the "continue" option and give it a name such as:

   ```
   espresence-mr60fda1-mmwave-sensor
   
   ```

* Click next and chose the type of ESP module you used in your build, this isn't a critical thing to have match but as long as it's some kind of ESP32 you can just select that for now and click next.
* You'll see a new card appear in the background for your ESP device, this is just an empty shell with only basic initialization code so far... click skip because you don't want to install this basic code to the ESP quite yet.
* Now click edit on your new sensor in ESPHome and you'll see the basic code:
   ```
   esphome:
    name: espresence-mr60fda1-mmwave-sensor

   esp32:
    board: esp32dev
    framework:
      type: arduino

   # Enable logging
   logger:

   # Enable Home Assistant API
   api:
     encryption:
       key: "cEEo6Dse5jSfuJ2FznX+3n7A6+6ZmzVNe92axpm2t04="

   ota:
     password: "488d6a3de442afaddb0250cffce64711"

   wifi:
    ssid: !secret wifi_ssid
    password: !secret wifi_password

     # Enable fallback hotspot (captive portal) in case wifi connection fails
    ap:
      ssid: "espresence-mr60fda1-mmwave-sensor"
     password: "RvZXGuhrPCzl"

   captive_portal:
   ```

* The easiest way to proceed is to copy all the code above out to notepad++ or your favorite editor and then paste back in the entire code from:
   ```
   esphome.yaml
   ```
* Now just edit some key lines in your new config:

   ```
    substitutions:
      # change device name to match your desired name
      device_name: "espresence-mr60fda1-mmwave-sensor"
      # change sensor name below to the one you want to see in Home Assistant
      device_name_pretty: ESP mmWave 60 gHz Test
      # change room name below to the one you want to see in Home Assistant
      room: ""
      # change the below to be your WiFi SSID
      ssid: !secret wifi_ssid
      # change the below to be your WiFi password
      wifi_password: !secret wifi_password
      # UART TX Pin
      uart_tx_pin: A0
      # UART RX Pin
      uart_rx_pin: A1
      
   ```
* The room: "name" is key as it will be the name of each sensor object in HA so if you chose "Office" here, you sensors will be Office Motion, Office Presence, etc...

* You'll also want to copy the generated api encryption key and ota password into this section of the full code:

   ```
   # Enable Home Assistant API
   api:
      encryption:
         key: "assigned_when_you_add_to_esphome"

   ota:
      password: "assigned_when_you_add_to_esphome"
   ```

* Lastly, in the wifi: section, there is a line that says "use_address: XXX.XXX.XXX.XXX" this is an optional element to workaround typical issues with mDNS and wifi/subnets. Basically, if you find your device is always showing as offline in ESPHome or has any issues at all when making changes or updating OTA, you'll want this setting. Note: this is not a static IP, it just tells ESPHome to use an IP address to do all OTA work with a given ESP device. You can of course set a static IP but this isn't that. You'll also notice that the wifi ssid and passwords are variables, you'll also see at the top of the full config are references to "secrets" file. You'll need to create one or update the one in this repo and copy it to the config/esphome main folder... if you have setup other esp devices, changes are you have this already.

   ```
   # Connect to WiFi & create captive portal and web server
   wifi:
      ssid: "${ssid}"
      password: "${wifi_password}"
      use_address: XXX.XXX.XXX.XXX
   ```
