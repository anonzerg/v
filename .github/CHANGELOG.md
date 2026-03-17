## What's Changed

### New features

* Add partial extended xyz support (https://github.com/briling/v/pull/18)
* Add `j` hotkey to jump to a frame inside and CLI option `frame:%d` to start with a specific frame (https://github.com/briling/v/pull/5)
* Add `bmax` CLI argument for max. bond length (920202b)
* Add `com` and `exitcom` CLI argument for `gui:0` and on-exit command sequences, respectively
* Add `colors` CLI argument for colorscheme (#29)
* Python integration (#28, see https://github.com/aligfellow/xyzrender, #35)

### Improvements
* New colors by @iribirii (#2, #13, #15, #29)
* Remove case sensitivity of xyz file inputs (#9)
* Add CLI option to disable centering of molecules (#14)
* Disable default rotation wrt inertia axis for z-matrix input and add a CLI option to force it (#14)
* Read molecules from the standard input (#28)
* Show infrared intensities and mode masses (#35)
* Improve the text-in-corner look (de242c7, #36)
* Improve data structures and code readability

### Fixes
* Exit correctly when window closed (#10)
* Fix chiral z-matrix input (#14)
* Fix NaNs when compute dihedrals (#14)
* Fix the "readagain" (`r`) and "readmore" (`tab`) bugs (#31)
* Fix z-matrix input with unit=bohr (8554864)
* Fix text blinking when playing animation (80cae88, #36)

### Coming in the next version:
* extended xyz (#16, #17)
* high-symmetry determination bugs (#21)
* how to build on mac (#34)

**Full Changelog**: https://github.com/briling/v/compare/v2.0...v3.0rc5

