*It was created using pico-sdk and C language.  

- This is the firmware for Raspberry Pico W.    
- Pico W sends the following sensor data via a TCP socket at 5-second intervals.      
    - GPIO input value    
    - ADC value (voltage value, Pico W temperature sensor value)    
    - Data from Bosch's BME280 (temperature, humidity, and air pressure sensor)*   
      *If the BME280 is not connected, the BME280 data will be 0.    
- The Pico W can act as both a TCP server and a TCP client.       
- A dedicated PC app is used to set up the wireless LAN for Pico W.
- Please refer to the manual for usage.    
