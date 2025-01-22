# Bravely Arranged Standard Library
`bastd` is my own custom standard library for C, made to aid in the development
of games, 3D software, 2D development tools and more.

It strays heavily from what is supported in the
[lackluster](https://nullprogram.com/blog/2023/02/11/) C standard library and
aims to be a sufficient and even superior replacement.

`bastd`'s design goals include:
- Simplifying the build system
- Providing a full framework for Graphical and Console Applications
- Giving clear errors and debugging tools to the Programmer
- Making C a more "tolerable" language

`bastd`'s features include:
- [x] **No header files.** They cause more harm than good in my opinion, and I
prefer a single translation unit in my builds. You just include `bastd.c` at
the top of your main C file and you start writing code.
- [x] **Memory Allocators**. These include the
[Arena Allocator](https://nullprogram.com/blog/2023/09/27/), which groups
allocations into a single lifetime for a certain region of code (i.e. a frame),
and a [Buddy Allocator](https://www.gingerbill.org/article/2021/12/02/memory-allocation-strategies-006/),
which is our general-purpose allocator in place of `malloc`, that has the benefit
of making the region of memory allocated to more explicit.
- [x] **A Length-Based String Type**. By adding one variable to accompany a
`char *`, almost all operations upon strings become infinitely easier. Said
operations are included by default.
- [x] Generic data-structures. The premier one is the
[Dynamic Array](https://dylanfalconer.com/articles/dynamic-arrays-in-c), which
brings the power of C++'s `std::vector` to C. 
- [ ] A I/O system that supports both standard input/output and file
input/output. It's built on a basic buffer type that you can flush to output
data. This buffer type also acts as a **string builder**, by letting you append
data to the buffer, then compile it to a string type.
- [ ] A logging system built ontop of this I/O system. You can either log out to
a file or to stdout.
- [ ] A simple GUI initialization system that gets you an OpenGL window with
immediate-mode input
- [ ] A basic 2D renderer built ontop of this graphics initializater, that can
draw shapes and textures to the screen. The shaders used can be customized for
added effect. It features a simple camera system to seperate which parts of
the screen use which shaders and viewports.
- [ ] A multi-threaded job system. You can also just use the cross-platform
primitives.
- [ ] An audio library. Windows implementation uses WASAPI, Linux uses ALSA.

---
- [x] implemented
- [ ] planned
---

The only currently supported platform is Windows, though once the APIs are
finished, a Linux and FreeBSD port are planned.

## Why C?
Cuz I like C. It's the closest "high-level" language to assembly, so I have
maximum control. It is *really simple*, which in some ways is to its detriment.
Almost all OS libraries, Graphics libraries, Audio libraries, etc. are written
in C.

C is great. Don't let anyone tell you it's not.

## Examples
You can change the build script to any of the examples. Once built, you should
step through each example in a debugger in order to see how the programs work.

## Setup
Download the [latest release](https://github.com/midnadimple/bastd/releases/latest)
whenever you want to start a new project.

All of your code is going to go in the root of this folder, and all the `bastd`
code has been kept in the `bastd` folder. FOr convenience, the `bastd.c` file,
which includes all other C files in the folder, stays in the top level
directory, so you can easily include the file.

So, open `main.c` and have a scroll through it to see some example code. Then,
delete everything in the file and enter this basic skeleton:
```c
#include "bastd.c"

CALLBACK Error
start(Slice_S8 args, Buffer stdin, Buffer stdout)
{
	return Error_success;
}
```

Now, you can compile the program with `build.bat` or `build.sh` to get an
executable.

Note that since you make a new copy of `bastd` for every project, the library
is completely vendored and you can modify it however you'd like.

## LICENSE
`bastd` is licensed under the 3-clause BSD license. See [LICENSE.md](LICENSE.md).