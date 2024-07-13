(1) The microcontroller board uses Raspberry Pico W.    
(2) Pico W sends the following sensor data via a TCP socket at 5-second intervals.    
    (a) GPIO input value  
    (b) ADC value (voltage value, Pico W temperature sensor value)  
    (c) Data from Bosch's BME280 (temperature, humidity, and air pressure sensor) *  
    *: If the BME280 is not connected, the data in (c) will be 0.  
(3) Pico W acts as a TCP server.  
(4) A wireless LAN router that supports the 2.4 GHz band Wi-Fi standard "IEEE 802.11b/g/n" is required.  
(5) A dedicated PC app is used to set up the wireless LAN for Pico W.  
