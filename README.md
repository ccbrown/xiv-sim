Welcome
=======

This is a combat simulator for Final Fantasy 14. You can find more info on its status at the <a href="http://ffxivguild.net/sim">ffxivguild.net/sim</a>.

If you're a fellow software developer and you're interested in adding models / rotations for other classes or creating a GUI or anything of that sort, please put in some pull requests. :)

How to Build
============

Install a recent version of GCC or Clang and run `make`.

How to Use
==========

The most useful feature is probably the ability to test thousands of iterations of a fight at different durations. To do that, run `simulator multi-iteration`.

For now, the stats and rotations are hardcoded. You can edit them in <i>applications/multi-iteration.cpp</i> and <i>rotations/Monk.cpp</i>.
