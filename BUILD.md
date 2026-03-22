# Build instructions

## GNU/Linux

### Requirements:

* `gcc >= 4.7`
* X11 dev pagages:
    * `libx11-dev libxpm-dev x11proto-dev` (Ubuntu / Debian / most of deb-based distros)
    * `libX11-devel libXpm-devel xproto-devel` (most of rpm-based distros)
* `make`

### Compilation

```
make
```

## macOS

thanks to [@aligfellow](https://github.com/aligfellow)

```
brew install --cask xquartz # runtime display
brew install libx11 libxpm pkg-config
make
```
