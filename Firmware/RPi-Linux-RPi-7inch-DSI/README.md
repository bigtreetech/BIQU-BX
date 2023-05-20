# RPi-7inch-DSI

## Support OS

|   Hardware   |    32-bit     |   64-bit     |
|  :---: | :-----------: | :----------: |
|  RPi-3 |    5.15.32    |  5.15.32   |
|  RPi-4 |    5.15.32    |   5.15.32    |

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
> * Different hardware and kernel versions correspond to different drivers.