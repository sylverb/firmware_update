# firmware_update

**firmware_update** is a tool for the Game & Watch console that runs entirely in RAM. This binary is aimed to be loaded by the bootloader available at https://github.com/sylverb/game-and-watch-bootloader

## Features

This application is detecting if an update file is present on the sd card and will install everything, giving information to the user about update progress. It handles update of files on the sd card and internal flash bank 2

```mermaid
graph TD
    A[Start] --> B[Detect SD Card hardware]
    B -->|No SD Card found| E[Boot application in Bank 2]
    B -->|SD Card detected| C[Extract /retro-go_update.bin]
    
    C -->|Extract Success| F[Delete /retro-go_update.bin]
    C -->|Extract Failed| G[Show error message]

    F --> D
    G --> D

    D[Check for /update_bank2.bin] --> |update_bank2.bin exists| H[Flash Bank 2 with new firmware and delete file]
    D -->|File not found| I[Unmount filesystem]

    H --> I

    I --> J{Text visible on screen?}
    J -->|Yes| K[Show 'Press any button to continue']
    J -->|No| E

    K --> L[Wait for button press]
    L --> E
```

## Getting Started

### Building the Project

Clone the repository:
   ```bash
   git clone https://github.com/sylverb/firmware_update.git
   cd firmware_update
   make download_sdk (needed on macOs only)
   make
```

To upload built file to the sd card using jtag, run
   ```bash
   make sdpush
```