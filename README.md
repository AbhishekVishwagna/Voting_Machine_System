# Voting Machine Project

This is a Voting Machine Project on the ARM Cortex-A9 (DE1-SoC) with real time LCD display, hardware button input, a countdown timer, audio feedback. 
This project involves the design and implementation of Voting Machine System. This Voting Machine System uses the DE1-SoC ARM/FPGA platform. The system has a few features such as 10 minute election window, individual 60 second voter sessions and debounced switch inputs for reliability. Then the results are shown on an LT24 LCD with custom graphics and seven segment displays the 60 second voter time. The system also has a audio feedback to confirm successful votes. 

---

## Objectives
- To Handle user input through hardware push buttons.
- To display an Interactive, real time election dashboard on the LT24 LCD.
- To Provide Immediate sound feedback (beeps) while voting.
- To manage the election lifecycle from IDLE to RESULT.

---
## Hardware Requirements

### Development Board
| Component | Detail |
|-----------|--------|
| Board | Terasic DE1-SoC |
| CPU | ARM Cortex-A9 |
| FPGA | Altera Cyclone V SoC |
| IDE | Arm Development Studio |



### Required Peripherals
| Peripheral | Usage in Project |
|------------|-----------------|
| LT24 LCD Screen (240×320px) | Election dashboard - Welcome, vote, and result display |
| KEY0 - KEY3 Buttons | Voting input and election start/reset |
| Interval Timer | Time countdown for voting period (ends election) |
| 7-Segment Display | Real-time countdown timer (in seconds) |
| LEDs on-board | Voting status indicators - on during `VOTING` period, blinking on close |
| Audio Codec (WM8731) | Election beep and fanfare on 3.5mm audio jack |
| USB-Blaster JTAG Cable | Program upload and debugging using Arm Development Studio |

---
  
## System Design
The ARM Cortex-A9 processor serves as the central controller for the voting machine. This processor manages the voting process from initialization to the final result display.

**Hardware Components**:The system incorporates three hardware peripherals.

- **Push Buttons (KEY0 to 3):** The processor uses these buttons to capture user votes. Each button corresponds to a specific candidate. Software debouncing prevents double voting.
- **LT24 LCD Display:** This screen provides real time monitoring. It displays candidate names, vote counts, a countdown timer, and the final results.
- **Audio Codec:** This module provides voter feedback. A beep confirms each successful vote, so the voter knows their input has been registered.

**Software Architecture**:The software uses a finite state machine implemented in C language. It operates in three states:


| State    | Description                                              |
|----------|----------------------------------------------------------|
| `IDLE`   | The system waits for the election to start. The LCD displays a welcome message.|
| `VOTING` | The buttons register votes. The software increments counts when voted and manages a countdown timer. |
| `RESULT` | The voting period ends. The software identifies the winner and displays data on the LCD. |

All peripheral devices are mapped into memory. The processor periodically reads the buttons, writes directly to the LCD controller via the LT24 interface, and plays audio via the audio codec's control registers the onboard.

The system operates as a finite state machine (FSM) with four distinct states. Each state transition is triggered either by direct user input via the hardware push buttons or automatically by the voting period countdown timer. The diagram below illustrates the full lifecycle of an election session.

```text
                                   KEY0 pressed
                         ┌─────────────────────────────┐
                         │                              ▼
                    +---------+                    +---------+
                    |         |                    |         |
                    |  IDLE   |                    |  VOTING |◄─────────────┐
                    | (Welcome|                    |         |               │
                    | Screen) |                    +---------+               │
                    +---------+                         │                    │
                                                        │ KEY1/KEY2/KEY3     │
                                                        │ pressed            │ Vote registered
                                                        ▼                    │ (beep plays)
                                                   +---------+               │
                                                   |  Vote   |───────────────┘
                                                   |Recorded |
                                                   +---------+
                                                        │
                                                        │ Timer expires
                                                        ▼
                                                   +---------+
                                                   |         |
                                                   | RESULT  |
                                                   | (Winner |
                                                   | Screen) |
                                                   +---------+
                                                        │
                                                        │ KEY0 pressed
                                                        ▼
                                                   Back to IDLE
```


> **State Summary**
> - `IDLE` — System is ready and awaiting election start
> - `VOTING` — Buttons active; votes are recorded; countdown running
> - `RESULT` — Timer has expired; winner is computed and displayed on LCD
> - Pressing KEY0 from `RESULT` resets all vote counts and returns to `IDLE`

---

## File Structure

```text
ELEC5620M-Voting-Machine/
│
├── Voting Machine System/          # Full system integration 
│   ├── audio.c                     # Audio IP initialisation, beep and fanfare implementation
│   ├── audio.h                     # Few Functions: play_vote_beep(), play_close_fanfare()
│   ├── lcd.h                       # Few Functions: lcd_showWelcome(), lcd_showCandidates(), lcd_showResults()
│   ├── lcd.c                       # Isolated test harness for LCD screen hardware testing
│   ├── machine.h                   # Shared header: candidate structs, state machine enum, defines
│   ├── main.c                      # Main program: Full system integration
│   ├── timer.c                     # Interval Timer countdown, expiry detection, 7-seg output
│   └── timer.h                     # Few Functions: SystemTimer_start(), VoterTimer_start()
│
├── Voting Machine Core/            # Core logic & integration 
│   ├── machine.h                   # Shared header: candidate structs, state machine enum, defines
│   └── main.c                      # Main program: FSM, vote engine, full system integration
│
├── LCD Display/                    # LCD display module
│   ├── lcd_display.h               # Few Functions: lcd_showWelcome(), lcd_showCandidates(), lcd_showResults()
│   └── lcd_main.c                  # Isolated test harness for LCD screen hardware testing
│
├── Timer and 7 Segment/            # Input, timer & 7-segment module
│   ├── timer.c                     # Interval Timer countdown, expiry detection, 7-seg output
│   ├── timer.h                     # Few Functions: SystemTimer_start(), VoterTimer_start()
│
├── Audio/                          # Audio module
│   ├── audio.c                     # Audio IP initialisation, beep and fanfare implementation
│   ├── audio.h                     # Few Functions: play_vote_beep(), play_close_fanfare()
│
├── .gitignore                      # Excludes build artefacts and IDE-generated files
└── README.md                       # Full project documentation
```

---
