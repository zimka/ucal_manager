UCal Manager
============

Description
-----------
Open source nanocalorimetry software package


Download
--------
::

  git clone https://bitbucket.org/zimka_b/ucal_manager
  cd ucal_manager
  git submodule init
  git submodule update


Build
-----
**For Linux:**

::

  mkdir .build
  cd .build
  cmake ../
  cmake --build .


*TODO*: update windows-part to be more specific
**For Windows:**
	* open project in VS2017
	* choose ucal_manager target
	* run build

Tests
------
In build folder:
::

    (<build folder>)$ ./tests/run_tests
