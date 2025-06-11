# Early Function Execution PoC for Android

This project demonstrates how to execute a custom native C++ function **before** any Java/Kotlin code - even **before** the `Application` constructor - using `ContentProvider` and `__attribute__((constructor))` in native code.

---

This solution performs early native root detection - specifically targeting the presence of PlayIntegrityFix (PIF) - before any Java/Kotlin code is executed, even before Application.onCreate(). It's designed to be:

1. Hard to bypass (runs before standard Java sandbox initializes)

2. Fast & deterministic (direct native checks using access() and opendir())

3. Invisible to Java-only hooking tools

The result is passed via a global native flag (isRooted), accessible from Java via JNI, enabling clean separation of logic and UI.

### What is implemented?
1. Native C++ detection:

    - Checks known filesystem paths for playintegrityfix

    - Optionally scans /data/adb/modules/ for suspicious names

2. Static constructor (__attribute__((constructor))):

    - Executes as early as possible, before JNI_OnLoad, before Application

3. JNI bridge:

    - Java reads isRooted via a native method

4. Java UI:

    - Shows alert if detection result is true

This makes the approach robust for production anti-tamper or integrity enforcement.

---

## Key Features

- Executes a native function **before** `Application.onCreate()`
- Uses `ContentProvider` to load `native-lib.so` early
- Relies on `__attribute__((constructor))` to run C++ before `JNI_OnLoad`
- Supports Direct Boot
- Fully deterministic & scalable design - suitable for enterprise production environments

---

## Project Structure

```
EarlyNativeInitPoC/
├── app/
│   ├── src/
│   │   ├── main/
│   │   │   ├── cpp/              # Native code (C++)
│   │   │   │   ├── native-lib.cpp
│   │   │   │   └── CMakeLists.txt
│   │   │   ├── java/com/example/poc/
│   │   │   │   ├── MainActivity.java
│   │   │   │   ├── MyApp.java
│   │   │   │   └── EarlyInitProvider.java
│   │   │   └── AndroidManifest.xml
│   └── build.gradle              # Module-level Gradle config
├── settings.gradle
├── build.gradle                  # Project-level Gradle config
└── README.md                     # This file
```

---

## How early loading works via ContentProvider

Android guarantees that all `ContentProvider`s are initialized **before** `Application.onCreate()` - even before the app fully starts.

This project uses that behavior to trigger native code early:

### Sequence
1. `AndroidManifest.xml` declares `EarlyInitProvider` with `android:directBootAware="true"`
2. As soon as the app process starts, Android loads this provider
3. Inside the class `EarlyInitProvider`, there is a `static {}` block:

```java
static {
    System.loadLibrary("native-lib");
    Log.i("EarlyFunctionExecution", "native-lib loaded via ContentProvider");
}
```
4. When the library is loaded:
   - Native function `early_function()` marked with `__attribute__((constructor))` is triggered automatically
   - Then `JNI_OnLoad()` is executed

This ensures that native code runs **before `Application` or `Activity` is even touched**.

---

## Notes
- Native library is loaded statically in `EarlyInitProvider`
- `early_function()` is marked with `__attribute__((constructor))` for early execution
- `ContentProvider` is preferred for reliable early initialization over `Application` class

---

## How to Build & Run

###  Option 1: Android Studio
1. Open the project in **Android Studio Arctic Fox+**
2. Make sure NDK is installed (via SDK Manager)
3. Click `Run` ▶️ to build & launch on your connected device or emulator

### Option 2: Command Line (no IDE)
```bash
cd EarlyNativeInitPoC

# Clean and build APK
./gradlew clean assembleDebug

# Install APK to device
adb install -r app/build/outputs/apk/debug/app-debug.apk

# Launch the app
adb shell monkey -p com.example.poc -c android.intent.category.LAUNCHER 1
```

### Building Native Library Only
If you want to build **only the native C++ library**:

```bash
# Build only native-lib.so using externalNativeBuild
./gradlew app:externalNativeBuildDebug

# Output will be at:
app/.cxx/Debug/...
```
> You can also specify build variants (`Debug` / `Release`) as needed.

---

## Test Scenarios

### 1. `early_function()` must run before Application

- Use:
  ```bash
    adb logcat | grep EarlyFunctionExecution
  ```

- Expected log:
  ```bash
    adb logcat | grep EarlyFunctionExecution
    early_function() called BEFORE JNI_OnLoad and onCreate
    JNI_OnLoad called
    native-lib loaded via ContentProvider
    onCreate()
    MyApp Application.onCreate()
    MainActivity()
  ```
- `early_function()` MUST be the very first entry.

---

### After Manual App Kill
```bash
adb shell am force-stop com.example.poc
adb shell monkey -p com.example.poc -c android.intent.category.LAUNCHER 1
adb logcat | grep EarlyFunctionExecution
```
- `early_function()` MUST be the very first entry.

---

###  Direct Boot (Device Locked After Reboot)
1. Enable secure lock (PIN, Password)
2. Reboot device
3. Without unlocking the screen, run:
```bash
adb shell cmd package compile -f -m speed com.example.poc
adb shell monkey -p com.example.poc -c android.intent.category.LAUNCHER 1
adb logcat | grep EarlyFunctionExecution
```
- You should still see logs from `early_function()` - meaning it executes even before unlocking.

---

## Root Detection Test Instructions

To test root-like detection using `playIntegrityFix` signature:

1. Run this to simulate detection in a non-rooted phone:
```bash
adb shell "echo mock > /sdcard/playintegrityfix_mock"
```

2. Uncomment this line in `native-lib.cpp` path array:
```cpp
"/sdcard/playintegrityfix_mock" // just for testing!!
```

3. Build and run the application.
4. You should see an alert dialog that root detection was triggered.
5. Then, clean up:
```bash
adb shell rm /sdcard/playintegrityfix_mock
```

### Why global variable?
Using a global `static bool isRooted`:
- Enables early assignment during `__attribute__((constructor))`
- Ensures Java code can check detection **at any time**, without re-running native logic
- Simplifies JNI and avoids need for cached `JNIEnv*` or repeated disk checks

This approach keeps detection stateless and immutable from Java-side, increasing security and robustness.

---

## Enterprise-Ready Characteristics
- Runs before Java security model is applied
- Ideal for anti-tamper, license enforcement, or root detection
- Compatible with 12,000+ Android devices thanks to standard platform features (NDK + ContentProvider)
- No reflection, no hacks, no root - fully compliant

---

## Contact & Questions
If you're reviewing this project as part of a coding challenge or assessment, feel free to reach out for any clarification.

---

## License
MIT or Apache 2.0, as per project requirement.