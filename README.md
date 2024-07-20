# Computer Systems Performance Simulation Using MVA

This project has been developed for the Computer Systems Performance course at NTUA. The goal of this project is to determine the throughput, residence time, utilization, and number of customers at each station of a computer system as described in the exercise. The system features two types of clients and a load-dependent station (Dd Server CPU), while every other station is load-independent. The report and exercise description are written in Greek.

![image](https://github.com/user-attachments/assets/039621bb-1501-4f27-8eb4-262918ccc4f1)

## Repository Contents

- **Exercise_Description.pdf**: Contains detailed information about the system parameters.
- **Report.pdf**: Includes the results of the simulations using both JMT and C++ methods.
- **simulation_jmt.jmva**: JMT/JMVA tool simulation file.
- **approximateMVA.cpp**: Implements the approximate MVA for the system described in the exercises, considering load-dependent stations.
- **multithreadedMVA.cpp** and **singleThreadedMVA.cpp**: Implement the accurate MVA algorithm for load-independent stations only.
