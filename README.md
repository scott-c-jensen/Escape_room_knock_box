# Escape_room_knock_box
An arduino controlled electronic lock box that stays locked until knocked in a specific pattern. This project contains the code, wiring diagram and a 3d model for mounting. In the end it will look something like this.

Here is the box:

<img src="https://user-images.githubusercontent.com/29553708/148718283-6bc4d581-e913-4ac2-99eb-40a168c99a2c.jpg" width=50% height=50%>

The electronics in the lid of the box enclosed in a 3d print:

<img src="https://user-images.githubusercontent.com/29553708/148718309-45da3590-3d4f-4917-9811-95cfdf687571.jpg" width=50% height=50%>

Removing the 3d printed cover:

<img src="https://user-images.githubusercontent.com/29553708/148718336-3f34b648-8668-452f-8b58-3d1406149245.jpg" width=50% height=50%>

Finally an up close picture of the electronics:

<img src="https://user-images.githubusercontent.com/29553708/148718371-1d022e31-ecde-41ee-9b70-ebf14f59f447.jpg" width=50% height=50%>

CODE BASE:
The code is written in arduino and has a number of global variables to control the sensitivity and knock pattern for the code. No special requirements for setup as there are no other dependencies. 

ELECTRONICS:
Here is the wiring diagram which includes the following:
  - Arduino nano
  - one 8x AA/AAA battery holder (or two 4x wired as shown in the diagram)
  - one TIP 120 transistor
  - one 1N4001 diode
  - one Piezo electric sensor
  - one 2.2 kΩ resistor
  - one solenoid lock (9-12V operation normally closed)
  - (optional) one protoboard 

<img src="https://user-images.githubusercontent.com/29553708/148709073-46046fa8-4154-4a4d-8cb2-e704124d9b02.png" width=50% height=50%>

The wiring schematic is also shown here (though the TIP 120 schematic from adafruit appears unreasonably large in the Fritzing program). The drawing is also included in the repo.

<img src="https://user-images.githubusercontent.com/29553708/148711272-a373ed85-852e-4f08-b7c7-d4c5df3c55f9.png" width=75% height=75%>

3D PRINTING/CONSTRUCTION:
For the 3d printed parts, glue the base plate to the top of the lid with the nuts (#4 and #6) captivated against the top of the box. There are spaces for 4 nuts but 2 opposite corners is enough to hold the electronics. After gluing, you will need to screw a hole for a #2 screw in the protoboard to screw the board to the base plate. The cap will then go on using #6 screws.

Also note that the latch mechanism needs something to catch onto when closed. This can be a simple thin sheet glued to the side of the box base. In my case the box
