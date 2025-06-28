# IoT-Integrated-Health-Monitoring-System-Featuring-Fall-Detection-and-Environmental-Sensing

This project aims to develop a comprehensive, IoT-based health monitoring system that can: 
• Monitor vital physiological indicators like body temperature, heart rate, SpO₂, and ECG
readings continuously.
• Use motion sensors to identify falls so that prompt action can be taken.
• Keep an eye on environmental factors like air quality, humidity, and room temperature.
• ESP32 enables real-time data transfer to a cloud interface, enabling remote monitoring and
access.
• Set off notifications or warnings in the event of an emergency or unusual readings.


This system promotes safer and more independent living, especially for the elderly and those with chronic illnesses, by combining environmental awareness, fall detection, and health sensing into a single platform.
The goal of this system is to provide real-time monitoring of physiological and environmental data via an IoT-based solution that is compact, inexpensive, and simple to use. The device, which may be used at home or in assisted living facilities, aids in the early detection of potential health issues, improves patient safety, and decreases the strain on carers.


Uploaded a photo for connections (for references)


This project introduces the design and development of an IoT-Integrated Health Monitoring System that integrates real-time vital signs monitoring, environmental monitoring, and fall detection into a small, wireless package. The main objective is to provide a non-invasive, low-cost, and easy-to-use system appropriate for remote healthcare monitoring, particularly for elderly patients, chronically ill patients, or those in need of continuous monitoring outside clinical environments. The platform is centered on the ESP32 microcontroller, which reads and sends information from a set of biomedical and environmental sensors. Health parameters like heart rate, SpO₂, body temperature, and ECG activity are read using the MAX30100, DS18B20, and AD8232 sensors. Environmental parameters like room temperature, humidity, and air quality are
recorded by the DHT11 and MQ-135 sensors. A gyroscope (MPU6050) facilitates in-situ fall detection, providing an essential safety function. All the data collected is processed by the ESP32 and sent wirelessly to a cloud platform, where it gets visualized using a friendly dashboard that can be accessed using mobile or web applications. Through the integration of IoT technology, sensor networks, and remote access features, this project provides a scalable and viable solution for smart healthcare applications, facilitating safer and more independent living and improving the capability of caregivers and healthcare professionals to remotely monitor patients.
