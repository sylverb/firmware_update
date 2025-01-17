# firmware_update

**firmware_update** is a tool for the Game & Watch console that runs entirely in RAM. This binary is aimed to be loaded by the bootloader available at https://github.com/sylverb/game-and-watch-bootloader

## Features

This application is detecting if an update file is present on the sd card and will install everything, giving information to the user about update progress. It handles update of files on the sd card and internal flash bank 2

```mermaid
graph TD
    A[Start] --> B[Detect SD Card hardware]
    B -->|No SD Card found| F[Boot application in Bank 2]
    B -->|SD Card detected| C[Check for /gw_update.tar]
    
    C -->|Found| D[Extract /gw_update.tar]
    C -->|Not Found| E[Check for /update_bank2.bin]
    
    D -->|Extract Success| G[Delete /gw_update.tar]
    D -->|Extract Failed| H[Show error message]

    G --> E
    H --> E

    E -->|/update_bank2.bin exists| I[Flash Bank 2 with new firmware]
    E -->|File not found| J[Unmount filesystem]

    I --> J

    J --> K{Text visible on screen?}
    K -->|Yes| L[Show 'Press any button to continue']
    K -->|No| F

    L --> M[Wait for button press]
    M --> F
```

## Getting Started

### Building the Project

Clone the repository:
   ```bash
   git clone https://github.com/sylverb/firmware_update.git
   cd firmware_update
   make
```

On macos you may have to run
   ```bash
   make download_sdk
```
before the make

To upload built file to the sd card using jtag, run
   ```bash
   make sdpush```