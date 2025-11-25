# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a Qt-compilable version of RTKLIB 2.4.3 b30, a high-precision GNSS positioning library supporting GPS, GLONASS, Galileo, BeiDou, QZSS, and IRNSS. The project provides both Qt GUI applications and command-line tools built on a shared C library core.

## Build System

### Qt Applications (Primary)

Build the entire project using Qt Creator or qmake:

```bash
# Build everything (library + all apps)
qmake RTKLib.pro
make

# Build only the core library
cd src
qmake src.pro
make

# Build specific Qt application
cd app/rtkpost_qt
qmake rtkpost_qt.pro
make
```

The build system uses a two-stage hierarchy:
1. `src/` builds first → produces `libRTKLib.a` (static library)
2. `app/` builds second → links against `libRTKLib.a`

Build artifacts appear in `build/Desktop_Qt_6_10_0-Debug/` (or similar, depending on Qt configuration).

### Console Applications (Alternative)

Console applications can be built standalone without Qt:

```bash
# Build rnx2rtkp (post-processing tool)
cd app/rnx2rtkp/gcc
make

# Run built-in tests (requires test data from RTKLIB)
make test

# Clean build artifacts
make clean

# Install to system
sudo make install  # Installs to /usr/local/bin
```

Other console apps follow the same pattern: `str2str/`, `convbin/`, `pos2kml/`, `rtkrcv/`.

Note: Test data referenced in makefiles (`../../../test/data/`) is not included in this repository. Tests will fail without it.

## Architecture

### Layered Design

```
Qt GUI Apps (rtkplot_qt, rtkpost_qt, rtkconv_qt, etc.)
    ↓ uses
Shared GUI Components (app/appcmn_qt/)
    ↓ uses
Core RTKLIB Library (src/libRTKLib.a) ← also used by console apps
    ↓ uses
Platform APIs (pthreads/WinAPI, BSD sockets/Winsock)
```

### Core Library (src/)

**Pure C library** with no Qt dependencies. Key modules:

- **rtkcmn.c** - Common utilities: time, coordinates, matrix operations, satellite geometry
- **rtkpos.c** - RTK relative positioning (carrier-phase ambiguity resolution)
- **postpos.c** - Post-processing positioning engine
- **ppp.c / ppp_ar.c** - Precise Point Positioning with ambiguity resolution
- **rinex.c** - RINEX observation/navigation file I/O (RINEX 2/3)
- **stream.c** - Data streams (files, TCP, serial, NTRIP client/server)
- **rtcm.c / rtcm2.c / rtcm3.c / rtcm3e.c** - RTCM message parsing (RTCM 2, RTCM 3, MSM, SSR)
- **preceph.c** - Precise ephemeris (SP3 format)
- **sbas.c** - SBAS corrections (WAAS, EGNOS, MSAS)
- **solution.c** - Solution output formatting
- **rcv/** - Receiver-specific binary format decoders (u-blox, NovAtel, Septentrio, Javad, etc.)

All functions declared in `src/rtklib.h` (338+ functions exported via `EXPORT` macro).

### Qt Applications

Six active GUI applications in `app/`:
- **rtklaunch_qt** - Application launcher
- **rtkplot_qt** - Solution plotting and analysis tool
- **rtkpost_qt** - Post-processing GUI (wrapper for postpos.c)
- **rtkconv_qt** - Data format conversion GUI
- **rtkget_qt** - Automatic GNSS data downloader
- **srctblbrows_qt** - NTRIP source table browser

Shared components in **app/appcmn_qt/**:
- Custom plotting widget (graph.cpp/h) - Full 2D graph engine
- Common dialogs (viewer, console, options, time/reference input)
- Google Maps integration (gmview)

All Qt apps follow this pattern:
1. Include `src/rtklib.h` for C API access
2. Link against `libRTKLib.a`
3. Use QThread for heavy processing (e.g., ProcessingThread in rtkpost_qt)
4. Store settings via QSettings
5. Reuse dialogs from appcmn_qt/

### Platform Support

**Linux:**
- Requires: libpng, pthread
- Optional: liblapack/libblas (for optimized linear algebra)
- Optional: Intel MKL (high-performance math)
- Includes IERS library (lib/iers/) for geodetic corrections

**Windows:**
- Requires: Ws2_32.lib (Winsock2), winmm.lib (multimedia timers)
- Visual Studio projects available in `vc++/`

**Qt Version:**
- Supports Qt 4.x, 5.x, 6.x
- Uses QtSerialPort for serial communication (Qt 5+)
- Web view options: QtWebEngine (modern) or QtWebKit (legacy)

## GNSS Feature Configuration

GNSS system support controlled via preprocessor defines in `RTKLib.pri`:

```
-DENAGLO    # Enable GLONASS
-DENAQZS    # Enable QZSS (Japan)
-DENAGAL    # Enable Galileo (Europe)
-DENACMP    # Enable BeiDou (China)
-DENAIRN    # Enable IRNSS (India)
-DNFREQ=3   # Support 3 carrier frequencies
-DNEXOBS=3  # Support 3 extended observation codes
-DEXTLEX    # Enable QZSS LEX corrections
-DTRACE     # Enable debug tracing
```

Additional advanced options:
- `-DLAPACK` - Use LAPACK for matrix operations
- `-DMKL` - Use Intel Math Kernel Library
- `-DIERS_MODEL` - Use GMF troposphere model instead of NMF
- `-DOBS_100HZ` - Support 100Hz observation rate

## Code Conventions

**Language:** Pure C (C89/ANSI C) for core library, C++ with Qt for GUI apps

**Naming:**
- Functions: lowercase with underscores (e.g., `rtkpos_()`, `readrnxobs()`)
- Structures: lowercase with `_t` suffix (e.g., `gtime_t`, `obs_t`, `rtk_t`)
- Preprocessor: UPPERCASE (e.g., `MAXOBS`, `NFREQ`)

**Memory Management:**
- Static allocation preferred where possible
- Dynamic allocation via `malloc()` - caller responsible for `free()`
- No C++ features in core library (no classes, exceptions, STL)

**Threading:**
- Core library is NOT thread-safe by design
- Applications handle thread synchronization
- Stream I/O uses platform threads (pthread on Linux, CreateThread on Windows)

**Error Handling:**
- Most functions return status code (0=error, 1=success) or pointer (NULL=error)
- Trace macros for debugging: `trace()`, `tracet()`, `traceobs()`, `tracenav()`

## File References in Code

When referencing code locations, use this format:
- Files: `src/rtkpos.c`
- Specific lines: `src/rtkpos.c:1523`
- Line ranges: `src/rtkpos.c:1523-1545`
- Functions typically have comment headers indicating their purpose

## Documentation

- **doc/manual_2.4.2.pdf** - Complete RTKLIB manual (6MB)
- **doc/relnote_2.4.2.htm** - Release notes with changelog
- Code comments in source files with version history
- Function documentation in `src/rtklib.h` header

## Common Modifications

**Adding GNSS System Support:**
1. Enable in `RTKLib.pri`: Add `-DENA<SYS>` define
2. May require updating `MAXSAT` in `src/rtklib.h`
3. Rebuild entire project (library + apps)

**Adding Receiver Format:**
1. Create decoder in `src/rcv/newreceiver.c`
2. Add to `src/src.pro` SOURCES
3. Register in `src/rcvraw.c:init_raw()` function
4. Implement `input_<format>()` and `input_<format>f()` functions

**Modifying Positioning Algorithm:**
- RTK: Edit `src/rtkpos.c` (main engine: `rtkpos()` function)
- PPP: Edit `src/ppp.c` and `src/ppp_ar.c`
- SPP: Edit `src/pntpos.c`
- Post-processing: Edit `src/postpos.c`

**GUI Customization:**
- Shared dialogs: Modify `app/appcmn_qt/` (affects all apps)
- App-specific: Edit individual app's .cpp/.h/.ui files
- Plotting: Customize `app/appcmn_qt/graph.cpp` (independent 2D engine)

## Important Notes

- This is a **Qt port** of the original RTKLIB by T. Takasu
- Upstream: https://github.com/tomojitakasu/RTKLIB
- The core library maintains compatibility with upstream RTKLIB
- Qt apps are specific to this fork
- No automated test suite included (manual testing via makefiles in console apps)
- Thread safety must be handled at application level
