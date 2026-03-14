# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Sailcut CAD is a Qt 6 application for designing boat sails and developing them into flat panels. It uses the qmake build system.

## Build Commands

```bash
# Build the application
qmake && make

# Build in a separate directory (recommended)
mkdir build && cd build && qmake .. && make
```

The built binary is output to `bin/sailcut` (Linux), `bin/Sailcut CAD.app` (macOS), or `bin/sailcut.exe` (Windows).

## Running Tests

```bash
# Run all tests
make check

# Run a specific test
./tests/geocpp/tst_geocpp
./tests/sailcpp/tst_sailcpp
```

Tests use the Qt Test framework and are located in `tests/geocpp/` and `tests/sailcpp/`.

## Architecture

The codebase is organized into two main libraries plus the UI layer:

### `src/geocpp/` - Linear Algebra Library
Core geometry types: `CVector3d`, `CPoint3d`, `CMatrix`, `CMatrix4x4`, `CSubSpace` (lines/planes), `CRect3d`. Used for all 3D calculations.

### `src/sailcpp/` - Sail Logic Library
- **`CSailDef`** - Sail parameters (dimensions, type, cut layout, cloth settings)
- **`CSailWorker`** - Sail calculations and panel layout algorithms (cross-cut, twist, vertical, radial, mitre, etc.)
- **`CPanel`** / **`CPanelGroup`** - Represents sail panels with edges (CSide) and labels
- **`CBoatDef`** / **`CBoatElement`** - Complete boat definition combining sails, hull, rig
- **`CHullDef`** / **`CRigDef`** - Hull and rig definitions

### UI Layer (`src/`)
- **`CSailApp`** - QApplication subclass handling preferences, translations, recent documents
- **`CFormMain`** - Main window with tabbed sail viewers
- **`CFormSail`**, **`CFormBoat`**, etc. - Dialogs for editing definitions
- **`CSailViewer`** - OpenGL widget for 3D sail display
- **`CSailDisp`** / **`CSailDispGL`** - Sail rendering

### File I/O (`src/`)
- **`CSailDoc`** - XML document handling (read/write sail definitions)
- **Export writers** - DXF, SVG, Carlson, TXT, XML formats in `sailwriter-*.cpp`

## File Formats

Sailcut uses XML-based file formats with specific extensions:
- `.saildef` - Sail definition
- `.hulldef` - Hull definition
- `.rigdef` - Rig definition
- `.boatdef` - Complete boat (multiple elements)
- `.panel3d` - 3D panel group

## Key Relationships

```
CSailDef → CSailWorker::makeSail() → CPanelGroup (3D sail panels)
CPanelGroup → flattening → CPanelGroup (2D developed panels for cutting)
```

The sail mould (`CSailMould`) defines the 3D shape via profile curves at different heights.

## Unit Preferences

Sailcut supports displaying and inputting dimensions in either millimeters (default) or inches.

- **Preference storage**: `CPrefs::useInches` (boolean, persisted in `~/.sailcutrc`)
- **Access via**: `CSailApp::useInches()` property
- **UI**: Edit > Preferences menu opens `CFormPrefs` dialog

When `useInches` is true:
- All dimension input fields display/accept inches
- Unit labels show "in (or ft'in\")" to indicate accepted formats
- Internal storage always remains in mm (conversion factor: 25.4)
- Computed values (Sail Area m², Diagonal) remain metric

### Imperial Input Formats

When using inches mode, the following input formats are supported:
- **Decimal inches**: `25.5`
- **Inches with fraction**: `25 1/2` (whole number, space, then fraction)
- **Fraction only**: `1/2`
- **Feet and inches**: `5'3` or `5'3"`
- **Feet and inches with fraction**: `5'3 1/2`

The input parsing is handled in `formsaildef.cpp` in the `readLength` lambda function.
