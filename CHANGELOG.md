Change Log
==========
Changes are documented here.

[v0.0.4] - Nov 11, 2016 - First stable release
-------------------------
### Added
  - test controller functions
  - serial command and monitoring support
    - nodejs interactive-cli and serial-monitor apps
  - cloud functions: bpt:probe, btp:test, and btp:reset
  - (internal) new data type: test_input_t
  - (internal) hasMoved and isMoving functions to BPT_Accel and BPT_GPS
  - (internal) BPT_GPS::getGpsCoord now returns the age the coordinate was polled

### Removed
  - (internal) controller modes CONTROLLER_MODE_SIM_AUTO and CONTROLLER_MODE_SIM_MANUAL in favor of CONTROLLER_MODE_TEST

### Fixed
  - all the controller states
  - (internal) module status functions in BPT_Accel and BPT_GPS
  - code cleanup

### Known Issues
  - after restet the controller doesn't correctly capture the first serial command


[v0.0.3] - Oct 29, 2016
-------------------------
### Added
  - cloud functions: bpt:gps, bpt:state, bpt:status
  - controller logic to publish bpt:events events
  - controller logic to receive bpt:ack events
  - accelerometer responds to movement events

[v0.0.2] - Oct 25, 2016
-------------------------
### Added
 - major refactoring and more controller logic
 - structural support for handling multiple remote devices
 - import failsafe binary
 - implement BPT_Accel_LIS3DH module
   - changes to clearInterrupt function


[v0.0.1] - Oct 19, 2016
-------------------------
### Added
  - first version (wip)
  - add logic to GPS_MT3339 module


[v0.0.4]: https://github.com/BP-Tracker/bp-tracker-particle/releases/tag/v0.0.4
[v0.0.3]: https://github.com/BP-Tracker/bp-tracker-particle/releases/tag/v0.0.3
[v0.0.2]: https://github.com/BP-Tracker/bp-tracker-particle/releases/tag/v0.0.2
[v0.0.1]: https://github.com/BP-Tracker/bp-tracker-particle/releases/tag/v0.0.1
