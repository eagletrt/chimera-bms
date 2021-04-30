# Chimera high voltage BMS  - hardware

## Libraries
The projects don't include the libraries since they use the KiCAD official ones.
Set accordingly the variables `${KICAD_SYMBOL_DIR}` and `${KISYSMOD}` for 
`kicad-symbols` and `kicad-footprints` respectively.
**For `kicad-symbols` checkout the repo to `v5` for now to have the legacy ones.**

Don't rescue symbols!


## Repo structure
```
chimera-bms:hw-master
|-- docs/       # Docs and datasheets of common interest
|-- mainboard/  # MainBoard hardware project folder
|-- cellboard/  # CellBoard hardware project folder
```
