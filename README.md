This project focuses on designing a portable wattmeter measurement device using the STM32F401CCU6 microcontroller, which is built around the ARM Cortex-M4 processor.

The developed prototype demonstrates two essential measurement capabilities:

1.Voltage Measurement: An analog voltage from a potentiometer connected to PA0 is sampled using the ADC and converted into its corresponding voltage level in millivolts.

2.Current Measurement: A small load circuit created using two 330-ohm series resistors is connected through an ACS712 Hall-effect current sensor, whose output is read on PA1. The sensor output is processed to compute real-time current in milliamps.

The measured values- both voltage and current are displayed on a 2×16 LCD module via I2C (using pins PB6 and PB7).

This project highlights the practical application of microcontrollers in measurement and instrumentation, showing how a compact STM32 platform can be used to build a portable, low-cost, and educational measurement tool
