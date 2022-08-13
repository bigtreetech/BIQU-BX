# RPi-7inch-DSI

## NOTE: This driver needs to be installed only after the motherboard and screen are upgraded to v3.0, because the interface of the screen is changed from HDMI to DSI. Ignore this step for previous versions of HDMI interface.

## Start installation

1. Download driver from Git

    ``` bash
    git clone --depth=1 -b RPi-7inch-DSI https://github.com/EchoHeim/RPi-Linux.git
    ```
    
2. Install the driver

    ``` bash
    cd RPi-Linux
    sudo ./install.sh
    ```

3. Wait for the system to restart, and then the display and touch will work normally.


> NOTE: 
> * The above steps need to ensure that the raspberry pi can be connected to the Internet normally.
> * When raspberry pi was started, the display of uboot was still 800*480 resolution, so the display was still abnormal when it was just started. The DSI output would not be normal until the system was running normally
> * Different hardware and kernel versions correspond to different drivers. Currently, we only provide drivers for PI4 kernel-5.15 32bit