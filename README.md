# EasyIRCRelay
EasyIRCRelay is a simple IRC bot that relays messages between channels on different servers.

# Requirements
 * QT 5 (with qmake)
	It can be easily installed on ubuntu using `sudo apt install qt5-default`.

# Installation
1. `qmake`
2. `make` (feel free to use `make -j8` in order for the build to be faster.)
3. Edit the `config.json` file.
4. `./EasyIRCRelay`

# TODO
 * Make global topic selection.
 * Make an init script to make it easier to start this app it the background.
 * Better Config error-checking.

<p>&nbsp;</p>

I would also like to thank the developers of LibCommuni for their awesome, easy-to-use IRC library!
The currently supported version is included in this repo, and is obviously copyrighted to it's owner.
