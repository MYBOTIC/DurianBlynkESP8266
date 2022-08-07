# DurianBlynkESP8266
Library for Durian UNO + ESP8266 Wi-Fi module (with Durian Blynk Firmware) connect to Blynk 0.1 or new Blynk App 

To get Durian UNO + ESP8266 Wi-Fi module (with Durian Blynk Firmware)
https://www.mybotic.com.my/durian-uno-enhancement-of-arduino-uno-for-iot-project-education?search=durian%20uno

ESP8266 Wi-Fi Module & Durian Uno

Durian Uno + ESP8266 Wi-Fi module (with Durian Blynk Firmware) is a dual-core processor that can work simultaneously in the same circuit. Both devices are working standalone.
-Durian Uno as a control unit to control input/output pins.
-with build in Durian Blynk Firmware, ESP8266 Wi-Fi module works as receive and transmit data to Blynk.
                  
Problem IoT Project when using ESP series controller and Solution with Durian UNO

Most of the sensors and modules use 5V as operating voltage, when using the ESP32/ESP8266 as the main controller, the extra logic converter is needed since the ESP32 / ESP8266 are using 3.3 V as operating voltage. For example, MQ series gas sensor is connected with ESP series controller and the gas sensor's operating voltage is 5V. It will effect the accuracy of reading data. There's another example, l298 motor driver with ESP series controller. The 3.3V operating voltage as a PWM signal makes the motor driver unable to maxima the output as the 5V PWM is the signal to drive maximum output.
Durian UNO's 5V operating voltage make it compatible with most of the sensors and modules. 

Most of the libraries are not yet completely compatible with the ESP series controller. Besides, the debugging of the library's issue is taking time. Therefore, those beginners who get started with IoT projects will get into trouble and perhaps will lose confidence in their project.
Durian Uno is compatible with the majority of sensor libraries and with the help of Durian Blynk ESP8266 library makes it easy to use and user-friendly. Durian UNO definitely builds the beginner's confidence and generates more interest in IoT projects. The great engineer will be born soon!

The while loop, delay function for the ESP series controller may cause error when using it for IOT project. 
Durian UNO and ESP8266 Wi-Fi module work independently. The while loop, delay function free to use without causing error in IOT project.
