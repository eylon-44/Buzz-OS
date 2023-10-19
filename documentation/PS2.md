**This is just a prototype to save some information and sources and will later be written fully and beautifuly**


The PS/2 controller has two (one byte) buffers for data — one buffer for data received from devices that is waiting to be read by your OS, and one for data written by your OS that is waiting to be sent to a PS/2 device. Most data sheets for PS/2 controllers are written from the perspective of the PS/2 device and not from the perspective of software running on the host. Because of this, the names given to these buffers are the opposite of what you expect: the output buffer contains a device's output data (data waiting to be read by software), and the input buffer contains a device's input (data that was sent by software).


**ports**
IO Port	Access Type	Purpose
0x60	Read/Write	Data Port
0x64	Read	Status Register
0x64	Write	Command Register

The Data Port (IO Port 0x60) is used for reading data that was received from a PS/2 device or from the PS/2 controller itself and writing data to a PS/2 device or to the PS/2 controller itself.

The Status Register contains various flags that show the state of the PS/2 controller.
https://wiki.osdev.org/%228042%22_PS/2_Controller#Status_Register

**command port**
The Command Port (IO Port 0x64) is used for sending commands to the PS/2 Controller (not to PS/2 devices).
The PS/2 Controller accepts commands and performs them. These commands should not be confused with bytes sent to a PS/2 device (e.g. keyboard, mouse).
To send a command to the controller, write the command byte to IO port 0x64. If there is a "next byte" (the command is 2 bytes) then the next byte needs to be written to IO Port 0x60 after making sure that the controller is ready for it (by making sure bit 1 of the Status Register is clear). If there is a response byte, then the response byte needs to be read from IO Port 0x60 after making sure it has arrived (by making sure bit 0 of the Status Register is set).