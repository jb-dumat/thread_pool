# thread_pool
A lite, generic and crossplatform threadpool in C++.
This project is tested by using google tests.

### Installing

You can use the source code only. If you want to build it or run the units tests you'll need to follow theses steps.

First create a build directory at the root of the repository:
```
mkdir build && cd build
```

Execute cmake to generate and then compile the project:
```
cmake ..
cmake --build .
```

The demo project is located in the 'project' directory
```
./project/thread_pool
```

The units tests are located in the 'tests' directory
```
./tests/units_tests
```

## Authors

* **Jean-Baptiste Dumat** - [jb-dumat](https://github.com/jb-dumat)
