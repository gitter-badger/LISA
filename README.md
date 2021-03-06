# Introduction

LISA is an acronym for "Lightweight Integrated System for Amateur Radio". It may also refer to the intelligent little girl from the Simpsons TV show - it's up to you. LISA provides a GUI framework for different modules related to amateur radio and linking them together in one integrated application environment.

All of this is based on the Qt libraries which ensure that LISA can be run on Windows, OSX and Linux.

# Plugins
The individual application functionality is provided through plugins, each of which focusses on a different task. 

## Available plugins
The following plugins are currently implemented and are worked upon:

- **Core** Provides core functionality for the main application.
- **Digital** Provides all features to be used during digital operation such as encoding/decoding digital protocols like PSK31, RTTY, etc. 

## Future plugins
Lots of different plugins can be thought of. Some of them include:

- **Logging** Manages a logbook and provides an interface to any other plugin interested in logbook data. This way, the digital module can display additional information about a worked callsign.
- **Control** Provides the functionality to control various kinds of transceivers. Possible backends are hamlib and RigCAT.
- **CloudStorage** Provides the ability to save and load various kinds of data from various kinds of cloud storages (i.e. Google Drive, Dropbox, Owncloud etc...)
- **CallsignLookup** Provides an interface to query callsign information from various callsign databases like QRZ.com
- **Rotator** Connects to a rotator device and allows the user to rotate his beams from within the application. Can also be used to align the beams for satellite operation

# Building
The application is based on the QtCreator source code, since it already provides a working application environment with a plugin ecosystem. Building is done through the QBS build system alone. To start developing, use the QtCreator IDE for your operating system and activate the QBS plugin to load the project. While the source code itself is platform independent, building has only been tested on Windows yet.

# Licensing
The application has been built on top of the QtCreator source code. Code that is based on QtCreator is licensed under LGPL 2.1. The LISA plugins themselves are licensed under GPL 3. Please respect the licensing at any time.

# Acknowledgements
LISA uses information and knowledge from the following open source projects:
- **fldigi**
- **GNURadio**
- **CuteSDR**

# Contributing
Contributing to the application is highly welcomed. If you want to contribute, contact the author via GitHub or issue a pull request. However, please note that contributing is not wise in the current development stage, since lots of concepts will change until everything is stable.
