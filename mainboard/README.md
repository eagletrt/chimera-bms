# MAIN BOARD
Formula SAE - unitn

## Authors
Latest first:
- Filippo Volpe
- Gregor Gombac
- Jesus probably

## Project structure
```
mainboard
|-- MainBoard.pro           # Project file
|-- MainBoard.sch           # Schematic root
|-- MainBoard.kicad_pcb     # Board layout file
|-- MainBoard-cache.lib     # Local cache of symbols
|-- fp-info-cache           # Cache of footprints
|-- fp-lib-table            # List of footprint lib files
|-- sym-lib-table           # List of symbols lib files
|-- lib_sch/                # Local symbol library files
|-- modules/                # Local symbol library files
|   |-- MainBoard.pretty/   # Local footprint library files
|   |-- packages3d/         # 3D models
|-- outputs/
|   |-- MainBoard.stp       # Board 3D step file
|   |-- images/             # Board renders and such
|   |-- gerber/             # Gerbers outputs
|   |-- netlists/           # Netlists outputs
|-- docs/                   # Additional related documents and outputs
|   |-- Decoupling.txt      # List of decoupling capacitor needed
|   |-- bom/                # BOM folder
```

## Notes
The board file is from V1, it has not been updated
