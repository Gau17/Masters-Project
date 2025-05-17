# Project Architecture


## Repo structure
```
Masters-Project/
├── README.md
├── LICENSE
├── docs/
│   ├── architecture.md  // For high-level system design
│   └── setup_guide.md   // For development environment setup instructions
├── esp32_controller/
│   ├── platformio.ini     // PlatformIO configuration for ESP32
│   ├── src/               // Source code for the ESP32
│   │   └── main.c         // Example main application file
│   ├── include/           // Header files for ESP32
│   │   └── .gitkeep
│   ├── lib/               // Local libraries for ESP32
│   │   └── .gitkeep
│   └── test/              // Tests for ESP32 code
│       └── .gitkeep
├── raspberry_pi_gateway/
│   ├── Makefile           // Makefile for building the Rust application & device drivers
│   ├── src/               // Source code for the Raspberry Pi (Rust)
│   │   └── main.rs        // Example main Rust application
│   ├── drivers/           // For any custom device drivers
│   │   └── Makefile       // Makefile specific to drivers
│   └── test/              // Tests for Raspberry Pi code
│       └── .gitkeep
└── cloud_platform/
    ├── src/               // Source code for cloud functions, data processing scripts
    │   └── .gitkeep
    ├── dashboard_config/  // Configuration files for Ubidots (or other) dashboard
    │   └── .gitkeep
    ├── ml_models/         // For larger ML models
    │   ├── data/          // Sample data, processed data for ML
    │   │   └── .gitkeep
    │   ├── notebooks/     // Jupyter notebooks for experimentation
    │   │   └── .gitkeep
    │   └── src/           // Source code for ML model training/inference
    │       └── .gitkeep
    └── test/              // Tests for cloud platform components
        └── .gitkeep
```
