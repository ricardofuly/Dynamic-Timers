# Dynamic Timers Component for Unreal Engine

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A robust, replicated Actor Component for Unreal Engine that manages network-synced timers using Gameplay Tags. It's a plug-and-play solution for creating timers with a full lifecycle (start, pause, resume), batch operations, and granular Join-In-Progress persistence rules.

---

## Table of Contents

- [Key Features](#-key-features)
- [Installation & Setup](#-installation--setup)
- [How to Use](#-how-to-use)
  - [1. Accessing the Component](#1-accessing-the-component)
  - [2. Managing Timers on the Server (Blueprints)](#2-managing-timers-on-the-server-blueprints)
  - [3. Displaying Timers on the Client UI (Blueprints)](#3-displaying-timers-on-the-client-ui-blueprints)
  - [4. Handling Join-In-Progress](#4-handling-join-in-progress)
- [API Reference (Blueprints)](#-api-reference-blueprints)
  - [Core Functions](#core-functions)
  - [Events (Delegates)](#events-delegates)
- [Contributions](#-contributions)
- [License](#-license)

---

## ✅ Key Features

-   **Component-Based:** Easily add to any `GameState` without changing its parent class.
-   **Network Replicated:** Built to run on the `GameState`, ensuring all timers are synchronized between the server and all clients.
-   **Gameplay Tag Driven:** Identify and manage timers using the flexibility of `GameplayTags`. Create unlimited timer types without changing C++ code.
-   **Full Lifecycle Control:** Functions to **Register**, **Start**, **Pause**, **Resume**, and **Remove** timers.
-   **Batch Operations:** Optimized functions to manage multiple timers with a single network call (`RegisterGlobalTimers`, `StartGlobalTimers`, etc.).
-   **Join-In-Progress (JIP) Support:** Define which timers should be visible to players who connect to a match already in progress.
-   **Blueprint-Friendly:** A clean and easy-to-use API for both server-side management and client-side UI display.

---

## 🚀 Installation & Setup

1.  Download (or clone) this repository.
2.  Create a `Plugins` folder at the root of your Unreal Engine project if one doesn't already exist.
3.  Copy the `DynamicTimers` plugin folder into the `Plugins` folder.
4.  Right-click your `.uproject` file and select `Generate Visual Studio project files`.
5.  Open your project. Unreal Engine will ask to compile the new plugin. Click "Yes".
6.  **Add the component to your `GameState`:**
    -   **For Blueprint GameStates:**
        1.  Open your `GameState` Blueprint.
        2.  Click the **`+ Add Component`** button.
        3.  Search for **`DynamicTimersComponent`** and add it.
        4.  Compile and save.
    -   **For C++ GameStates:**
        1.  In your `GameState.h` header file, include the component and declare it:
            ```cpp
            #pragma once

            #include "CoreMinimal.h"
            #include "GameFramework/GameStateBase.h"
            #include "MyGameState.generated.h"

            class UDynamicTimersComponent; // Forward declare

            UCLASS()
            class YOURPROJECT_API AMyGameState : public AGameStateBase
            {
                GENERATED_BODY()

            public:
                AMyGameState();

                UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timers")
                UDynamicTimersComponent* TimerComponent;
            };
            ```
        2. In your `GameState.cpp` constructor, create the component:
            ```cpp
            #include "MyGameState.h"
            #include "DynamicTimers/Public/DynamicTimersComponent.h" // Include the component

            AMyGameState::AMyGameState()
            {
                TimerComponent = CreateDefaultSubobject<UDynamicTimersComponent>(TEXT("DynamicTimersComponent"));
            }
            ```

The plugin is now set up and ready to use!

---

## 🔧 How to Use

### 1. Accessing the Component

To call functions or bind to events, you first need a reference to the `DynamicTimersComponent` from your `GameState`.

### 2. Managing Timers on the Server (Blueprints)

All actions that modify timers (register, start, etc.) **must be executed on the server** (e.g., in the `GameMode` or from a `Run on Server` event).

**Example: Registering and starting timers in the `GameMode`**

```blueprint
# In your GameMode's Event BeginPlay

# 1. Get the GameState and the DynamicTimersComponent
Get Game State -> Get Component by Class (Class: DynamicTimersComponent)

# 2. Prepare an array of timers to register in batch
Make Array
  -> Make BulkTimerRegistrationData (Tag: Match.TimeRemaining, Duration: 900.0, Persist: True)
  -> Make BulkTimerRegistrationData (Tag: Round.PreStartTime, Duration: 15.0, Persist: False)

# 3. Call the register function on the component
-> Register Global Timers

# 4. Later, in another event (e.g., "All Players Ready"), start a timer
-> Start Global Timer (Tag: Round.PreStartTime)
```

### 3. Displaying Timers on the Client UI (Blueprints)

The UI should be purely reactive. It listens for events from the `DynamicTimersComponent` to create, destroy, and update timer widgets.

**Example setup in your HUD Widget (`WBP_HUD`):**

1.  **On `Event Construct`:**
    -   Get the `GameState` and then the `DynamicTimersComponent`.
    -   Use `Bind Event` to subscribe to delegates like `OnTimerRegistered`, `OnTimerFinished`, etc.

    

2.  **Reacting to Events:**
    -   **`OnTimerRegistered`:** Create a timer widget (`WBP_TimerDisplay`), add it to the screen, and store its reference in a `Map` using the `GameplayTag` as the key.
    -   **`OnTimerFinished`:** Use the `GameplayTag` to find the widget in your `Map`, then remove it from the screen and the `Map`.

3.  **Updating Time (`WBP_TimerDisplay`):**
    -   In your timer widget's `Event Tick`, get the `DynamicTimersComponent` and call `GetTimerRemainingTime`, then update the text on screen. This function handles all states (registered, active, paused) automatically.

### 4. Handling Join-In-Progress

The system already synchronizes time correctly for new players. You just need to filter which timers are displayed by using the JIP flag in your HUD's `Event Construct`.

**Example JIP filter in `WBP_HUD`:**

```blueprint
# In Event Construct, after getting the DynamicTimersComponent

Get Active Timers -> For Each Loop
  -> Break FReplicatedTimerData
  -> Branch (Condition: bPersistForJoinInProgress)
    -> True: Create the timer widget for this player.
    -> False: Do nothing (this timer is ignored for the JIP player).
```

---

## 📖 API Reference (Blueprints)

*All functions are called on the `DynamicTimersComponent` instance.*

### Core Functions
*(All modifying functions must be called on the server)*

| Function                   | Description                                                                         |
| -------------------------- | ----------------------------------------------------------------------------------- |
| `RegisterGlobalTimer(s)`   | Adds one or more timers to the list, leaving them ready to be started.              |
| `StartGlobalTimer(s)`      | Begins the countdown for one or more previously registered timers.                  |
| `PauseGlobalTimer(s)`      | Pauses the countdown for one or more active timers.                                 |
| `ResumeGlobalTimer(s)`     | Resumes the countdown for one or more paused timers.                                |
| `RemoveGlobalTimer`        | Completely removes a timer from the system.                                         |
| `GetTimerRemainingTime`    | **(Client-safe)** Returns the remaining time for a specific timer.                  |
| `GetActiveTimers`          | **(Client-safe)** Returns the full array of current timer data structs.             |

### Events (Delegates)
*(Broadcast from the component and can be bound to anywhere, especially the UI)*

| Event                 | Fired when...                                                                   |
| --------------------- | ------------------------------------------------------------------------------- |
| `OnTimerRegistered`   | A new timer is registered and ready.                                            |
| `OnTimerStarted`      | A timer begins its countdown.                                                   |
| `OnTimerPaused`       | A timer is paused.                                                              |
| `OnTimerResumed`      | A paused timer is resumed.                                                      |
| `OnTimerFinished`     | A timer completes its countdown or is removed.                                  |

---

## 🤝 Contributions

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

---

## 📜 License

This project is licensed under the [MIT License](https://opensource.org/licenses/MIT).
