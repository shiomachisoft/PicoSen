*It was created using pico-sdk and C language without using MicroPython or Arduino IDE.  

- The microcontroller board uses Raspberry Pico W.    
- Pico W sends the following sensor data via a TCP socket at 5-second intervals.      
    - GPIO input value    
    - ADC value (voltage value, Pico W temperature sensor value)    
    - Data from Bosch's BME280 (temperature, humidity, and air pressure sensor)*   
      *If the BME280 is not connected, the BME280 data will be 0.    
- Pico W acts as a TCP server.        
- A dedicated PC app is used to set up the wireless LAN for Pico W.
- Please refer to the manual for usage.    
