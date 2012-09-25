BoostExamples
=============
A (messy) sandpit for experimenting with the Boost C++ libraries.

Requirements
------------
The main development takes place on Unix (Linux, Mac OS X), so to build
BoostExamples you will need the following software

* C++ compiler (GNU, Clang)
* [CMake](http://www.cmake.org) build tool, version 2.8.0 and higher
* CMake compatible build system (Make, Xcode, Eclipse)
* [Boost](http://www.boost.org) 1.48.0 or higher

How to Configure, Build and Test the Example Apps
-------------------------------------------------
Create a directory in which to build `BoostExamples`, change into
this directory and run CMake, pointing it to the `BoostExamples` source
tree (in other words, the directory in which this README resides):

```shell
$ mkdir BoostExamples-build
$ cd BoostExamples-build
$ cmake /path/to/BoostExamples
```

If CMake does not locate Boost correctly, you can use the `Boost_DIR`
argument to direct it to the root of your Boost installation:

```shell
$ cmake -DBoost_DIR=/boost/root/dir /path/to/BoostExamples
```

If configuration completes without error, CMake will have generated
a Makefile in the build directory. You can then build the
`BoostExamples` applications by running

```shell
$ make -jN
```

where `N` is the number of parallel build jobs. 

At this time, none of the targets are installed (they are examples
after all!).

The above description assumes a Unix style system, where CMake will 
generate Unix Makefiles by default. If you prefer working with another 
build tool, CMake is capable of generating projects for a wide range 
of IDEs and command line tools. Please see the [CMake Documentation on 
Generators](http://cmake.org/cmake/help/v2.8.9/cmake.html#section_Generators) for help on supported build tools and creating projects for
these tools using CMake. Note that available generators are platform and
CMake version dependent!
 

