# Real Time System Mini Project – Precise Periodic Real-Time Threads  

## 📌 Introduction  
This project demonstrates the use of **POSIX Threads (PThreads)** and **real-time scheduling** to create multiple periodic threads within a single process. The goal is to simulate real-time data acquisition (temperature and pressure) and synchronized display using precise timing constraints.  

The project focuses on:  
- Implementing **multi-threading** using `pthread_create()` and `pthread_join()`.  
- Enforcing **real-time scheduling** with POSIX RT APIs for deterministic timing.  
- Synchronizing threads to ensure periodic sampling and display.  

---

## 🎯 Objectives  
- Develop three real-time periodic threads in C using the **PThread library**.  
- Simulate temperature and pressure data acquisition with random value generation.  
- Ensure real-time periodic execution using **POSIX RT scheduling**.  
- Synchronize data exchange between worker threads and the display thread.  

---

## ⚙️ System Design  

### Threads Implemented  
1. **Temperature Thread**  
   - Samples temperature using:  
     ```c
     temperature = LOWER_TEMP_BOUND + (int)(UPPER_TEMP_BOUND * rand() / (RAND_MAX + 1.0));
     ```
   - Runs every **5 seconds**.  
   - Returns data to the main thread for synchronization.  

2. **Pressure Thread**  
   - Samples pressure using:  
     ```c
     pressure = LOWER_PRESSURE_BOUND + (int)(UPPER_PRESSURE_BOUND * rand() / (RAND_MAX + 1.0));
     ```  
   - Runs every **5 seconds**.  
   - Returns data to the main thread.  

3. **Display Thread**  
   - Receives both temperature and pressure values.  
   - Displays them on screen every **5 seconds**.  

### Scheduler  
- POSIX **real-time scheduling** (`SCHED_FIFO` or `SCHED_RR`) is applied.  
- Ensures **deterministic timing** and reduces jitter in periodic tasks.  

---

## 🛠️ Methodology  
1. Use **`pthread_create()`** to initialize three threads.  
2. Apply **real-time attributes** (`sched_param`) for precise periodic execution.  
3. Use **timers / nanosleep** to enforce strict periodic behavior.  
4. Implement inter-thread communication to share sampled values with the display thread.  
5. Synchronize output at every 5 seconds.  

---

## ✅ Conclusion  
- Successfully implemented **periodic real-time threads** using POSIX RT APIs.  
- Demonstrated **multi-thread synchronization** and real-time scheduling.  
- Acquired simulated **temperature and pressure values** and displayed them periodically.  
- This project enhanced understanding of **real-time programming concepts**, deterministic scheduling, and thread synchronization.  

---

## 📜 License
This project is licensed under the [MIT License](./LICENSE).  

---

## 🙌 Acknowledgements
- Developed as part of **Digital Electronics course project**.  
- Thanks to instructors and teammates for valuable feedback and collaboration.  

---
