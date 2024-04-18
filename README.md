# PlantWateringAutomationIoT
Plant Watering Automation is project to automate domestic rooftop garden with IoT and hardware devices.


### Compenents 
- ESP 32 Dev Borad
- DS 1307 RTC Clock 
- Cpacitive Soil Moisture
- 5V Relay Module
- 12v DC Pump
- DHT 11 Moisture Sensor
- DC Buck Converter



### Initial version  --v1.1

*Localised Automation*

- Objective 
 - To have automated system which is capable of watering a plant automatically with a time frame along with the loop response of soil Moisture sensor

- Explanation   
 - This code with simple logics, the use of lbraries made the job even more easier
 - Implemented with an AND logic of Conditions 
    - Soil Moisture Should be less than the threshold
    - Time form the RTC clock must be Specified two hour .i.e (7 AM or 6 PM )

