# Dynamic Timers Component for Unreal Engine

[![License: MIT](https://imgshields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A robust, replicated Actor Component for Unreal Engine that manages network-synced timers using Gameplay Tags. It's a plug-and-play solution for creating timers with a full lifecycle (start, pause, resume), batch operations, and granular Join-In-Progress persistence rules.

---

## Table of Contents

- [Key Features](#-key-features)
- [Installation & Setup](#-installation--setup)
- [How to Use](#-how-to-use)
  - [1. Accessing the Component](#1-accessing-the-component)
  - [2. Managing Timers on the Server](#2-managing-timers-on-the-server-blueprints)
  - [3. Using the Example Widget `W_Timer` (Simple)](#3-using-the-example-widget-w_timer-simple)
  - [4. Displaying Multiple Timers (Advanced)](#4-displaying-multiple-timers-advanced)
  - [5. Handling Join-In-Progress](#5-handling-join-in-progress)
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

**Example: Registering timers in the `GameMode`**

```blueprint
# In your GameMode's Event BeginPlay

# 1. Get the GameState and the DynamicTimersComponent
Get Game State -> Get Component by Class (Class: DynamicTimersComponent)

# 2. Prepare an array of timers to register
Make Array
  -> Make BulkTimerRegistrationData (Tag: Match.TimeRemaining, Duration: 900.0, Persist: True)
  -> Make BulkTimerRegistrationData (Tag: Round.PreStartTime, Duration: 15.0, Persist: False)

# 3. Call the register function on the component
-> Register Global Timers
```

### 3. Using the Example Widget `W_Timer` (Simple)

The plugin includes a ready-to-use example widget, `W_Timer`, designed to display a **single, specific timer**. This is the fastest way to get a timer on screen, perfect for a main match clock, a bomb countdown, or any prominent timer.

You can find it in the plugin's content folder: `/DynamicTimers/Content/Widgets/`

**How to use it:**

1.  Open your main HUD widget where you want the timer to appear.
2.  In the `Palette` panel, find `W_Timer` under the `[User Created]` category and drag it onto your design canvas.
3.  Select the `W_Timer` you just added.
4.  In the `Details` panel on the right, find the category named **`Dynamic Timers -> Settings`**.
5.  Set the **`Filter Tag`** property to the exact `GameplayTag` of the timer you want this widget to display (e.g., `Match.TimeRemaining`).

That's it! This widget will now automatically listen for events related to that specific tag and handle its own visibility and time updates.

### 4. Displaying Multiple Timers (Advanced)

This approach is for dynamically showing *all* active timers, creating and destroying widgets as needed. It's more complex but offers maximum flexibility.

**Example setup in your HUD Widget (`WBP_HUD`):**

1.  **On `Event Construct`:** Get the `DynamicTimersComponent` and use `Bind Event` to subscribe to delegates like `OnTimerRegistered`, `OnTimerFinished`, etc.
2.  **Reacting to Events:**
    -   **`OnTimerRegistered`:** Create a timer display widget, add it to a container (like a `Vertical Box`), and store its reference in a `Map`.
    -   **`OnTimerFinished`:** Use the `GameplayTag` to find the widget in your `Map`, then remove it from the screen and the `Map`.

### 5. Handling Join-In-Progress

The system already synchronizes time correctly for new players.

-   If you are using the example widget **`W_Timer`**, it handles its own JIP logic automatically based on its Filter Tag. No extra work is needed.
-   If you are using the **advanced method** (Displaying Multiple Timers), you must filter which timers are displayed by checking the JIP flag in your HUD's `Event Construct`.

**Example JIP filter for the advanced method:**

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
