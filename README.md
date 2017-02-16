BP Tracker Firmware
==========

Provides a particle.io [cloud API][cloudapi] to monitor and receive alerts when a device moves away from a dynamic geofence.

[![Build Unstable][shield-unstable]](#)
[![MIT licensed][shield-license]](#)



Table Of Contents
-----------------

- [Intro](#intro)
- [Features](#features)
- [Requirements](#requirements)
- [Usage](#usage)
- [Cloud API](#cloud-api)
- [Support & Releases](#support-&-releases)
- [Additional Tools](#additional-tools)
- [License](#license)

Intro
-------

TODO

Features
-------
  * Automatic device arming and disarming
  * State change notifications and `PANIC` settings
  * Persistent configuration settings
  * GPS polling and low-battery functions
  * Support for multiple remote clients
  * Message delivery handshake
  * Serial monitoring and testing (via nodejs cli)


Requirements
-------
BP Tracker requires the following:

  * particle.io [electron][electron] core (tested with 0.5.3 firmware)
  * [AssetTracker][assetrackershield] shield (tested with v002)
  * [particle cli][particlecli]

Usage
-----

The firmware can be flashed on the hardware using the particle cli:

```sh
particle compile electron --target 0.5.3 ./electron
particle flash --serial firmware_electron_xxxxxxxxxxxxx.bin
```

Then you can make queries on the API to configure and interrogate the device.
For example:

```sh
# get the GPS coordinates in a bpt:gps event
curl https://api.particle.io/v1/devices/<device_name>/bpt:gps -d access_token=<token>

```

Cloud API
-----

BP tracker utilizes the [Particle][particleio] cloud service to interface with client applications. The table below summaries all the functions
that can be called on a device running this firmware.

| Event          | Function |
| -------------- | -------- |
| `bpt:ack`      | Acknowledges certain events produced by the controller |
| `bpt:diag`     | Reports diagnostic information |
| `bpt:gps`      | Returns the geolocation of the device |
| `bpt:probe`    | Wakes up the controller to perform any pending actions |
| `bpt:register` | Register remote devices and configure settings |
| `bpt:reset`    | Resets the device |
| `bpt:state`    | Returns the controller's current state |
| `bpt:status`   | Reports the status of the device |
| `bpt:test`     | Puts the controller into testing mode |

Consult the [documentation][firmwaredocs] (in progress) for more information.

Additional Tools
-----

#### Serial Monitor

Monitors and prints on the command line events published to the device.
Using the hardware serial interface the firmware prints out the event using `Serial.print`.

```sh
cd tools/nodejs
node serial-monitor.js
```

<img width="400px" src="https://rawgit.com/BP-Tracker/bp-tracker-particle/master/docs/src/assets/images/serial-monitor.png" alt="Serial monitor screenshot" />

#### Interactive CLI

Publishes events to the device via serial usb as if the command had arrived from the cloud.

```sh
cd tools/nodejs
node interactive-cli.js
```

<img width="400px" src="https://rawgit.com/BP-Tracker/bp-tracker-particle/master/docs/src/assets/images/interactive-cli.png" alt="Interactive cli screenshot" />
<img width="400px" src="https://rawgit.com/BP-Tracker/bp-tracker-particle/master/docs/src/assets/images/interactive-cli-2.png" alt="Interactive cli screenshot" />


Support and Releases
-----

No releases yet.


License
-------

BP Tracker Firmware is licensed under the [MIT][info-license] license.  
Copyright &copy; 2016-2017 Derek Benda


[shield-unstable]: https://img.shields.io/badge/build-unstable-red.svg
[shield-license]: https://img.shields.io/badge/license-MIT-blue.svg

[firmwaredocs]: https://bp-tracker.github.io/bp-tracker-particle
[particlecli]: https://docs.particle.io/guide/getting-started/connect/electron/
[particleio]: https://www.particle.io/
[electron]: https://www.particle.io/products/hardware/electron-cellular-dev-kit
[cloudapi]: https://docs.particle.io/reference/api/
[assetrackershield]: https://docs.particle.io/datasheets/particle-shields/#electron-asset-tracker
[info-license]: LICENSE
