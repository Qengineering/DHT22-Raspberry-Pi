# DHT22 sensor on Raspberry Pi
![output image]( https://qengineering.eu/images/Sensor.webp )<br/>
## The DHT22 sensor software for a Raspberry Pi with WiringPi<br/>
[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)<br/><br/>

The DHT22 is a temperature and relative humidity sensor. It has a digital converter on board, which communicates with the Raspberry Pi via a 1-wire interface.

----------------

## Hardware connections.
The sensor requires a 3.3V power supply, available at pin 1 of the RPi. Do not connect the sensor to the 5V power supply. It will not break, but the 'high' signals from the Raspberry are 3.3V and, in this case, are not detected. In the diagram, the output is connected to pin 3, but can be connected to most GPIO pins as the pin is defined in the software.<br/><br/>
![output image]( https://qengineering.eu/images/WiringRPi.webp )<br/>

-----------------

## 1-Wire communication.
The DHT22 has its own rather obscure 1-wire interface protocol. It is very different from the standard Dallas 1-wire protocol supported by the Raspberry Pi GPIOs.<br/><br/>
![output image]( https://qengineering.eu/images/PulsTrain.png )<br/>
The transfer is initiated by a low of 18 mS, followed by a high of 40 µS to give the sensor some time to set up the communication flow. At the same time, the GPIO pin must be configured as an input to receive the pulse train from the sensors.
As can be seen, the length of a high determines a 0 or a 1. So the software comes down to measuring pulse lengths. It can be done through interrupt or polling techniques. Since interrupts are cumbersome and error-prone, it is better to use polling. It means continuously fetching the level of the input pin. When a change occurs, the elapsed time is measured.<br/><br/>
Accurately measuring microseconds is also another concern. We modified Daniel's software. He uses the WiringPi delayMicroseconds() function. As shown below, not the most accurate solution. We use the clock_gettime() function, which turns out to be much better.
Because the Raspberry Pi operating system runs many tasks and threads simultaneously, it happens regularly that a transition is missed and subsequently the outcome errors. It is marked by the boolean Valid.<br/><br/>
Below are a few histograms showing the difference between the WiringPi and the clock_gettime() function.
First, only the application runs without other demanding processes.<br/>
![output image]( https://qengineering.eu/images/600.webp )<br/>
Next, the same situation but now with different apps.<br/>
![output image]( https://qengineering.eu/images/600Multi.webp )<br/>
Finally, when the Raspberry Pi underclocked to 200MHz, something WiringPi's delayMicroseconds() brings into serious problems.<br/>
![output image]( https://qengineering.eu/images/200.webp )
