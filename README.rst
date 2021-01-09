UCal Manager
============

Description
-----------
Open source nanocalorimetry software package. 
Controls custom hardware based on IOtech DaqBoard3000USB. To use software with the hardware you must have IOtech drivers and libraries installed. Hardware can be used with Windows only because of the drivers.
Linux builds are possible with mock device for development and tests.
This repo contains the server, Python client can be found `here <https://bitbucket.org/zimka_b/ucal_client>`_ .

Prepare
--------
::

  git clone https://bitbucket.org/zimka_b/ucal_manager
  cd ucal_manager
  git submodule init
  git submodule update

You will also need to install or pull as submodules `grpc <https://github.com/grpc/grpc>`_ dependencies at third_party/grpc.
Please follow grpc installation `instructions <https://github.com/grpc/grpc/tree/master/src/cpp>`_ .


Build
-----
Two build mods are available: for a real hardware and for a mock hardware. The real hardware build is available on Windows only. Please place your daqboard drivers (Daqx.h) at third_party/daqx/ and the library (DAQX.lib) at lib/. Build mod is controlled by CMake variable USE_REAL_DEVICE, default is true.


**For Linux:**

::

  mkdir .build
  cd .build
  cmake ../
  cmake --build .


**For Windows:**

::

  - open project in VS2017 as CMake project
  - choose ucal_manager_server target
  - run build

Tests
------
In build folder:

**For Linux:**

::

    (<build folder>)$ ./tests/run_tests


**For Windows:**

::

  - open project in VS2017 as CMake project
  - choose run_ucal_tests target
  - run target

On Windows *run_hardware_tests* target is available, which uses actual Daqboard device to run tests.
Please check tests/run_hardware_tests.cpp for specific test-time hardware requirements.
