Welcome
=======

This is a combat simulator for Final Fantasy 14. You can take it for a test drive or find more info on its status at <a href="http://sim.ffxivguild.net">sim.ffxivguild.net</a>.

If you're a fellow software developer and you're interested in adding models / rotations for other classes or creating a GUI or anything of that sort, please put in some pull requests.

How to Build
============

Install a recent version of GCC or Clang and LLVM 3.4, and run `make`.

How to Use
==========

`time ./simulator single-json subjects/monk-bis.conf rotations/monk.sl 660`

Runs a single 660 second simulation and spits out the detailed results formatted via JSON.

`time ./simulator thorough-json subjects/monk-bis.conf rotations/monk.sl`

Runs several hundred thousand simulations and spits out the results formatted via JSON.