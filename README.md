# EasyIRCRelay
EasyIRCRelay is a simple IRC bot that relays messages between channels on different servers.

# Requirements
 * QT 5.4 (with qmake)

# Installation
1. `qmake` (Build options can be changed here)
2. `make` (feel free to use `make -j8` in order for the build to be faster.)
3. `sudo make install`
4. `sudo update-rc.d EasyIRCRelay defaults` (if you want to the relay to start on boot)

# TODO
 * ~~Make global topic selection.~~
 * Change topic on all networks after rehash.
 * ~~Make an init script to make it easier to start this app it the background.~~
 * Better Config error-checking.

<p>&nbsp;</p>

I would also like to thank the developers of LibCommuni for their awesome, easy-to-use IRC library!
The currently supported version is included in this repo, and is obviously copyrighted to it's owner.
