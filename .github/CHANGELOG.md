## What's Changed

### New features

* Add partial extended xyz support (https://github.com/briling/v/pull/18)
* Add `j` hotkey to jump to a frame inside and CLI option `frame:%d` to start with a specific frame (https://github.com/briling/v/pull/5)
* Add `bmax` CLI argument for max. bond length (920202b)
* Add `com` and `exitcom` CLI argument for `gui:0` and on-exit command sequences, respectively
* Python integration (#28, see https://github.com/aligfellow/xyzrender)

### Improvements
* New colors by @iribirii (https://github.com/briling/v/pull/2, https://github.com/briling/v/pull/13, https://github.com/briling/v/pull/15)
* Remove case sensitivity of xyz file inputs (https://github.com/briling/v/pull/9)
* Add CLI option to disable centering of molecules (https://github.com/briling/v/pull/14)
* Disable default rotation wrt inertia axis for z-matrix input and add a CLI option to force it (https://github.com/briling/v/pull/14)
* Read molecules from the standard input

### Fixes
* Exit correctly when window closed (https://github.com/briling/v/pull/10)
* Fix chiral z-matrix input (https://github.com/briling/v/pull/14)
* Fix NaNs when compute dihedrals (https://github.com/briling/v/pull/14)

### Still have to be done:
* finish colors (#15)
* ? extended xyz (#16, #17)
* ? fix `readmore` bug (#7)
* ? high-symmetry determination bugs (#21)
* ? how to build on mac

**Full Changelog**: https://github.com/briling/v/compare/v2.0...v3.0rc3

