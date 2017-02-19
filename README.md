## Just for fun Linux vehicle

### HTTP Web Server
* compile: `make`
* run: `./webserv port` on server
* open a browser or http client and send requests in the form: `ip-address-of-server:port/request?arg1?arg2` on the client

### Vehicle
* see `project.pdf` for project idea/basis

##### Raspberry Pi:
* add line `dtparam=i2c_arm=on` to `/boot/config.txt` to enable i2c in the Pi's device tree
* configure `i2c-dev` and `i2c-bcm2708` kernel modules to be loaded at boot: for example, add `i2c-dev` and `i2c-bcm2708` on separate lines to `/etc/modules-load.d/raspberrypi2c.conf`
* reboot and run `i2cdetect -y 0` after hooking up the i2c connection in order to see the hardware address (make sure to have `i2c-tools` installed)
    * we assume that pins 3(data) and 5(clock) are used on the RPI, you must also have a ground between the two boards

##### Arduino:
* install a [motor shield](https://www.adafruit.com/product/81) onto the board
* install the [Arduino IDE](https://www.arduino.cc/en/Main/Software) and the [AFMotor library](https://learn.adafruit.com/adafruit-motor-shield/library-install)
    * on Linux, you can use `install_libs.sh` to install the AFMotor libraries (it assumes the IDE is installed to `/usr/share/arduino`)
* compile `motor/drive/drive.ino` and upload it to the Arduino using the IDE
* connect wires from Raspberry Pi to Arduino (SDA to SDA, SCL to SCL, and ground to ground)

##### Actually Moving the Vehicle
* you can run `./run.cgi number` (with root privileges) on the Raspberry Pi to move the vehicle
    * number can be 1, 2, 3, or 4 to represent forward, backward, left, and right respectively
* rather than ssh, you can also start the webserver `./webserv port` and then send http requests to move the car, for example: `111.111.11.1:8080/run.cgi?2` will move the car backward

### Things We Learned
* C socket communication
* Perl scripting and gnuplot
* i2c communication on Raspberry Pi and Arduino platforms
* control of peripherals from Arduino (motors)
* nuances of basic wiring/circuitry- apparently having a ground ***is*** really important (who knew :stuck_out_tongue:)

### For The Future (/If We Had Time)
* autonomous driving mode
* live video feed
* desktop/smartphone application (would be trivial with current http request model)
