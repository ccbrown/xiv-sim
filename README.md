Welcome
=======

This is a combat simulator for Final Fantasy 14. You can take it for a test drive or find more info on its status at <a href="http://sim.ffxivguild.net">sim.ffxivguild.net</a>.

If you're a fellow software developer and you're interested in adding models / rotations for other classes or creating a GUI or anything of that sort, please put in some pull requests.

How to Build
============

Install a recent version of GCC or Clang and LLVM 3.4, and run `make`.

On a fresh Ubuntu machine, the steps should go about like this:

* Install the `add-apt-repository` command: `sudo apt-get install python-software-properties`
* Add the LLVM repository. Go to http://llvm.org/apt/ to figure out which repo is right for your system, then run `sudo add-apt-repository "deb http://llvm.org/apt/precise/ llvm-toolchain-precise-3.4 main"` with the correct repository.
* Add the toolchain PPA: `sudo add-apt-repository ppa:ubuntu-toolchain-r/test`
* Install the GPG key for LLVM: `wget -O - http://llvm.org/apt/llvm-snapshot.gpg.key|sudo apt-key add -`
* Update your sources: `sudo apt-get update`
* Install the rest of the prerequisites: `sudo apt-get install git make llvm-3.4-dev gcc-4.8 clang-3.4 libz-dev`
* Download the source: `git clone https://github.com/ccbrown/xiv-sim.git`
* Enter the source directory: `cd xiv-sim`
* Build: `CXX=c++ LLVM_CONFIG=llvm-config-3.4 make` (You can put those two variables in your profile to reduce the build command to simply `make`.)

How to Use
==========

`time ./simulator single-json --length 660 player subjects/monk-bis.conf rotations/monk.sl`

Runs a single 660 second simulation and spits out the detailed results formatted via JSON.

`time ./simulator thorough-json subjects/monk-bis.conf rotations/monk.sl 360 660`

Runs several hundred thousand simulations of lengths from 360 to 660 seconds and spits out the results formatted via JSON.