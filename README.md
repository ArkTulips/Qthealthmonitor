#  Qt Health Monitor Dashboard

##  Overview

This project presents a **Qt-based Patient Monitoring Dashboard** developed in C++ to demonstrate how UI responsiveness can be improved using asynchronous execution.

The system simulates real-time healthcare monitoring and highlights a common issue in GUI applications — **UI freezing due to blocking operations** — and provides an effective solution using multithreading.

---

##  Objectives

- Analyze responsiveness issues in GUI applications
- Demonstrate UI freezing caused by blocking operations
- Implement asynchronous execution using threads
- Improve user experience with real-time updates
- Evaluate system performance before and after optimization

---

##  Problem Statement

In applications built using Qt, the **event loop runs on the main thread**.  
When heavy operations (like file loading or computation) are executed on this thread, the UI becomes unresponsive.

This is critical in healthcare systems where:
- Continuous monitoring is required
- Delays can lead to missed critical information

---

##  Technologies Used

- C++
- Qt Framework (Qt Widgets)
- QThread (for multithreading)
- CMake (build system)

---

##  Features

###  Real-Time Monitoring
- Simulates:
  - Heart Rate
  - Oxygen Level
  - Temperature
- Updates every second using timers

---

###  Blocking System (Before)
- "Load Patient History" runs on main thread
- Causes:
  - UI freeze
  - Stopped updates
  - Unresponsive interface

---

###  Asynchronous System (After)
- Heavy task moved to background thread
- UI remains responsive
- Continuous data updates

---

###  Progress Tracking
- Progress bar shows task completion
- Status messages improve user feedback

---

###  File Simulation
- Reads from `patient_data.txt`
- Simulates real-world data processing

---

##  System Workflow

###  Before (Blocking)

User Action → Heavy Task (Main Thread) → Event Loop Blocked → UI Freeze

###  After (Async)

User Action → Worker Thread → Event Loop Active → Smooth UI

---

##  Implementation Details

###  Async Execution

A custom wrapper (`AsyncTask`) is used:

```cpp
AsyncTask::run([this]() {
    loadPatientHistory();

    QMetaObject::invokeMethod(this, [this]() {
        ui->statusLabel->setText("Done");
    });
});```

##Authors
###Sukrit Pal
###Divyanshu

