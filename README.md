## CMake Tutorial

> 主要参考：**[CMake Tutorial 3.21.4](https://cmake.org/cmake/help/v3.21/guide/tutorial/index.html)**
>
> 相关代码仓库：**[CMake-Tutorial](https://gitee.com/duycc/CMake-Tutorial)**

### Step1 简单的开始

最基本的项目是从源代码构建可执行程序。对于简单的项目，三行的`CMakeLists.txt`文件是必需的。这是此指南的开始。在step1创建如下的`CMakeLists.txt`：

```cmake
cmake_minimum_required(VERSION 3.10)

# set the project name
project(Tutorial)

# add the executable(Tutorial tutorial.cxx)
```

注意到在此文件中所有命令都是小写形式，其实大写，小写还有混合写CMake都是支持的。源文件tutotial.cxx将计算一个数的平方根：

```cpp
// A simple program that computes the square root of a number
#include <cmath>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " number" << std::endl;
    return 1;
  }
  double inputVal = std::atof(argv[1]);
  double outputVal = std::sqrt(inputVal);
  std::cout << "The square root of " << inputVal << " is " << outputVal << std::endl;
  return 0;
}
```

#### 添加版本号以及配置头文件

第一个特性是我们将为可执行程序和项目添加一个版本号。在源代码中也可以做到这个，但是用`CMakeLists.txt`文件控制更加灵活。首先，修改`CMakeLists.txt`文件，使用`project()`命令设置项目名和版本号。

```cmake
cmake_minimum_required(VERSION 3.10)

# set the project name
project(Tutorial VERSION 1.0)
```

然后，配置一个头文件将版本号传递给源代码：

```cmake
configure_file(TutorialConfig.h.in TutorialConfig.h)
```

因为配置文件将会被写入二进制树，我们必须添加一个列出头文件的路径。将下面这行添加到CMakeLists.txt文件的末尾：

```cmake
target_include_directories(Tutorial PUBLIC "${PROJECT_BINARY_DIR}")
```

用你喜欢的编辑器，在源码目录下创建TutorialConfig.h.in文件：

```
// the configured options and settings for Tutorial
#define Tutorial_VERSION_MAJOR @Tutorial_VERSION_MAJOR@
#define Tutorial_VERSION_MINOR @Tutorial_VERSION_MINOR@
```

CMake在配置过程中，将从CMakeLists文件中找到并替换头文件中`@Tutorial_VERSION_MAJOR@`和`@Tutorial_VERSION_MINOR@`

接下来修改tutorial.cxx文件，包含配置头文件TutorialConfig.h

最后，更新tutorial.cxx文件，让我们打印出可执行程序名以及版本号。如下：

```cpp
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " number" << std::endl;

    // report version
    std::cout << argv[0] << " Version " << Tutorial_VERSION_MAJOR << "." << Tutorial_VERSION_MINOR << std::endl;
    return 1;
  }
```

#### 指定C++标准

接下来在项目中加入一些C++11的新特性，将tutorial.cxx中的`atof`用`std::stod`替换，同时移除头文件`#include <cstdlib>`

```cpp
const double inputVal = std::stod(argv[1]);
```

我们需要在CMake源码中显式声明编译选项。指定C++标准最早的方式是使用`CMAKE_CXX_STANDARD`变量。在此指南中，设置[`CMAKE_CXX_STANDARD`](https://cmake.org/cmake/help/v3.21/variable/CMAKE_CXX_STANDARD.html#variable:CMAKE_CXX_STANDARD)为11并且将[`CMAKE_CXX_STANDARD_REQUIRED`](https://cmake.org/cmake/help/v3.21/variable/CMAKE_CXX_STANDARD_REQUIRED.html#variable:CMAKE_CXX_STANDARD_REQUIRED) 选项置为 `True`。确保此声明在调用`add_executable`之前。

```cmake
cmake_minimum_required(VERSION 3.10)

# set the project name
project(Tutorial VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
```

#### 构建和测试

在命令行模式下，可以创建一个`build`目录来执行构建，如下命令：

```bash
mkdir build
cd build
cmake ../step1/
```

初始化完成，就可以编译此项目：

```
cmake --build . # 或者直接 make 
```

测试如下：

```bash
duyong@Mac: build (main) $ ./Tutorial 10
The square root of 10 is 3.16228
duyong@Mac: build (main) $ ./Tutorial 444444
The square root of 444444 is 666.666
duyong@Mac: build (main) $ ./Tutorial 
Usage: ./Tutorial number
./Tutorial Version 1.0
```

> 源代码：[step1](https://gitee.com/duycc/CMake-Tutorial/tree/main/step1)

### Step2 添加一个库

现在给我们的项目添加一个库。这个库包含我们自己计算一个数平方根的实现。这个可执行程序将使用这个库而不是标准库里面的求平方根函数。

我们将库文件放到子目录MathFunctions里。这个目录包含一个头文件`MathFunctions.h`和一个源文件`mysqrt.cxx`。这个源文件包含一个`mysqrt`函数提供了标准库函数`sqrt`类似的功能。

在MathFunctions目录下添加如下`CMakeLists.txt`文件：

```cmake
add_library(MathFunctions mysqrt.cxx)
```

为了使用新库我们需要在顶层的CMakeLists.txt中调用[`add_subdirectory()`](https://cmake.org/cmake/help/v3.21/command/add_subdirectory.html#command:add_subdirectory)从而构建这个库。将新库添加到项目中，并且将MathFunctions目录包含进来保证可以找到`mysqrt.h`。如下：

```cmake
# add the MathFunctions library
add_subdirectory(MathFunctions)

# add the executable
add_executable(Tutorial tutorial.cxx)
target_link_libraries(Tutorial PUBLIC MathFunctions)

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
target_include_directories(Tutorial PUBLIC "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/MathFunctions")
```

现在设置MathFunctions库选项，虽然在此指南中这个不是必须的，但是在大型项目中是非常有用的。首先在顶层CMakeLists.txt添加一个选项：

```cmake
option(USE_MYMATH "Use tutorial provided math implementation" ON)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file(TutorialConfig.h.in TutorialConfig.h)
```

这个选项在使用[`cmake-gui`](https://cmake.org/cmake/help/v3.21/manual/cmake-gui.1.html#manual:cmake-gui(1)) 和 [`ccmake`](https://cmake.org/cmake/help/v3.21/manual/ccmake.1.html#manual:ccmake(1))时会有默认值ON，并且这个值用户可以修改。这个设置会存储在缓存中用户不必每次去设定这个选项值。接下来更改以有条件地使用MathFunctions库，修改顶层`CMakeLists.txt`：

```cmake
if(USE_MYMATH)
    add_subdirectory(MathFunctions)
    list(APPEND EXTRA_LIBS MathFunctions)
    list(APPEND EXTRA_INCLUDES "${PROJECT_SOURCE_DIR}/MathFunctions")
endif()

# add the executable
add_executable(Tutorial tutorial.cxx)
target_link_libraries(Tutorial PUBLIC ${EXTRA_LIBS})

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
target_include_directories(Tutorial PUBLIC "${PROJECT_BINARY_DIR}" ${EXTRA_INCLUDES})
```

变量EXTRA_LIBS收集可选库以便后续链接到可执行程序。变量EXTRA_INCLUDES用于可选的头文件。这是一种有多个可选项的典型处理方法。需要修改的源代码如下：

```c++
// tutorial.cxx
#ifdef USE_MYMATH
#include "MathFunctions.h"
#endif

// ...
#ifdef USE_MYMATH
  const double outputVal = mysqrt(inputVal);
#else
  const double outputVal = std::sqrt(inputVal);
#endif
```

在TutorialConfig.h.in中添加如下行：

```
#cmakedefine USE_MYMATH
```

接下来构建并运行该项目，测试选项

> 源代码：[step2](https://gitee.com/duycc/CMake-Tutorial/tree/main/step2)

### Step3 对库增加使用要求

增加使用要求可以对库或者可执行程序的链接实现更好的控制。主要有如下命令：

- [`target_compile_definitions()`](https://cmake.org/cmake/help/v3.21/command/target_compile_definitions.html#command:target_compile_definitions)
- [`target_compile_options()`](https://cmake.org/cmake/help/v3.21/command/target_compile_options.html#command:target_compile_options)
- [`target_include_directories()`](https://cmake.org/cmake/help/v3.21/command/target_include_directories.html#command:target_include_directories)
- [`target_link_libraries()`](https://cmake.org/cmake/help/v3.21/command/target_link_libraries.html#command:target_link_libraries)

让我们使用现代的CMake方法重构我们的代码。首先声明任何链接MathFunctions需要包含当前的源文件目录。因此变成一个有使用要求的接口。INTERFACE是消费者要求的而生产者不需要。将下列行加入`MathFunctions/CMakeLists.txt`中：

```cmake
target_include_directories(MathFunctions INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
```

现在我们已经指定了库MathFunctions的使用要求，我们可以安全地从CMakeLists.txt顶层移除EXTRA_INCLUDES变量，如下修改：

```cmake 
if(USE_MYMATH)
    add_subdirectory(MathFunctions)
    list(APPEND EXTRA_LIBS MathFunctions)
endif()

# ...

target_include_directories(Tutorial PUBLIC "${PROJECT_BINARY_DIR}")
```

重新构建测试

> 源代码：[step3](https://gitee.com/duycc/CMake-Tutorial/tree/main/step3)

### Step4 安装和测试

#### 安装规则

安装规则相当简单。对于MathFunctions库，我们设添加了这个库，将以下两行添加到MathFunctions/CMakeLists.txt：

```cmake
install(TARGETS MathFunctions DESTINATION lib)
install(FILES MathFunctions.h DESTINATION include)
```

然后在根目录下的CMakeLists.txt文件中添加如下行，用来安装可执行文件和配置文件：

```cmake
install(TARGETS Tutorial DESTINATION bin)
install(FILES "${PROJECT_BINARY_DIR}/TutorialConfig.h" DESTINATION include)
```

重新构建并执行`make install`：

```bash
duyong@Mac: build (main) $ make install 
Scanning dependencies of target MathFunctions
[ 25%] Building CXX object MathFunctions/CMakeFiles/MathFunctions.dir/mysqrt.cxx.o
[ 50%] Linking CXX static library libMathFunctions.a
[ 50%] Built target MathFunctions
Scanning dependencies of target Tutorial
[ 75%] Building CXX object CMakeFiles/Tutorial.dir/tutorial.cxx.o
[100%] Linking CXX executable Tutorial
[100%] Built target Tutorial
Install the project...
-- Install configuration: ""
-- Installing: /usr/local/bin/Tutorial
-- Installing: /usr/local/include/TutorialConfig.h
-- Installing: /usr/local/lib/libMathFunctions.a
-- Installing: /usr/local/include/MathFunctions.h
```

可以看到各个文件的安装路径，当然也可以通过[`CMAKE_INSTALL_PREFIX`](https://cmake.org/cmake/help/v3.21/variable/CMAKE_INSTALL_PREFIX.html#variable:CMAKE_INSTALL_PREFIX)来指定安装路径。

#### 添加测试

在根目录下的CMakeLists.txt文件最后添加如下代码来测试输入参数后产生的结果是否正确。

```cmake
enable_testing()

# does the application run
add_test(NAME Runs COMMAND Tutorial 25)

# does the usage message work?
add_test(NAME Usage COMMAND Tutorial)
set_tests_properties(Usage PROPERTIES PASS_REGULAR_EXPRESSION "Usage:.*number")

# define a function to simplify adding tests
function(do_test target arg result)
    add_test(NAME Comp${arg} COMMAND ${target} ${arg})
    set_tests_properties(Comp${arg} PROPERTIES PASS_REGULAR_EXPRESSION ${result})
endfunction(do_test)

# do a bunch of result based tests
do_test(Tutorial 4 "4 is 2")
do_test(Tutorial 9 "9 is 3")
do_test(Tutorial 5 "5 is 2.236")
do_test(Tutorial 7 "7 is 2.645")
do_test(Tutorial 25 "25 is 5")
do_test(Tutorial -25 "-25 is [-nan|nan|0]")
do_test(Tutorial 0.0001 "0.0001 is 0.01")
```

第一个测试仅是验证程序是否正常运行，没有语法错误或者崩溃，返回值为零。这是最基本的CTest形式。第二个测试使用[`PASS_REGULAR_EXPRESSION`](https://cmake.org/cmake/help/v3.21/prop_test/PASS_REGULAR_EXPRESSION.html#prop_test:PASS_REGULAR_EXPRESSION)测试输出是否包含指定的字符串。在这个测试例中，测试没有正确传递参数时是否输出了该程序的帮助信息。最后，定义了一个函数`do_test`，来测试各组测试例是否正确执行。

重新构建，并执行`ctest`命令，输出如下，可以看到测试例全部通过。

```bash
duyong@Mac: build (main) $ ctest 
Test project /Users/duyong/Study/CMake-Tutorial/build
    Start 1: Runs
1/9 Test #1: Runs .............................   Passed    0.00 sec
    Start 2: Usage
2/9 Test #2: Usage ............................   Passed    0.00 sec
    Start 3: Comp4
3/9 Test #3: Comp4 ............................   Passed    0.00 sec
    Start 4: Comp9
4/9 Test #4: Comp9 ............................   Passed    0.00 sec
    Start 5: Comp5
5/9 Test #5: Comp5 ............................   Passed    0.00 sec
    Start 6: Comp7
6/9 Test #6: Comp7 ............................   Passed    0.00 sec
    Start 7: Comp25
7/9 Test #7: Comp25 ...........................   Passed    0.00 sec
    Start 8: Comp-25
8/9 Test #8: Comp-25 ..........................   Passed    0.00 sec
    Start 9: Comp0.0001
9/9 Test #9: Comp0.0001 .......................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 9

Total Test time (real) =   0.04 sec
duyong@Mac: build (main) $ 
```

> 源代码：[step4](https://gitee.com/duycc/CMake-Tutorial/tree/main/step4)

### Step5 添加系统自检

考虑在该项目中添加一些代码，这些代码依赖目标平台没有的特性。例如，添加一些代码依赖目标平台是否有`log`和`exp`函数，当然，几乎每个平台都有这些功能，但本教程假设它们不常见。如果平台有log库，那么我们将使用它来计算平方根，而不使用mysqrt中的函数。我们首先在MathFunctions/CMakeLists.txt使用[`CheckSymbolExists`](https://cmake.org/cmake/help/v3.21/module/CheckSymbolExists.html#module:CheckSymbolExists)模块测试可行性。一些平台上，我们需要链接m库，如果log和exp找不到，尝试链接m库：

```cmake
include(CheckSymbolExists)
check_symbol_exists(log "math.h" HAVE_LOG)
check_symbol_exists(exp "math.h" HAVE_EXP)
if(NOT (HAVE_LOG AND HAVE_EXP))
    unset(HAVE_LOG CACHE)
    unset(HAVE_EXP CACHE)
    set(CMAKE_REQUIRED_LIBRARIES "m")
    check_symbol_exists(log "math.h" HAVE_LOG)
    check_symbol_exists(exp "math.h" HAVE_EXP)
    if(HAVE_LOG AND HAVE_EXP)
        target_link_libraries(MathFunctions PRIVATE m)
    endif()
endif()

if(HAVE_LOG AND HAVE_EXP)
    target_compile_definitions(MathFunctions PRIVATE "HAVE_LOG" "HAVE_EXP")
endif()
```

如果log和exp可用，将使用它们计算平方根。修改MathFunctions/mysqrt.cxx文件：

```cpp
  double result;
#if defined(HAVE_LOG) && defined(HAVE_EXP)
  result = exp(log(x) * 0.5);
  std::cout << "Computing sqrt of " << x << " to be " << result << " using log and exp" << std::endl;
#else
  double delta;
  result = x;

  // do ten iterations
  for (int i = 0; i < 10; ++i) {
    if (result <= 0) {
      result = 0.1;
    }
    delta = x - (result * result);
    result = result + 0.5 * delta / result;
    std::cout << "Computing sqrt of " << x << " to be " << result << std::endl;
  }
#endif
```

重新构建并执行：

```bash
duyong@Mac: build (main) $ ./Tutorial 16
Computing sqrt of 16 to be 4 using log and exp
The square root of 16 is 4
duyong@Mac: build (main) $ 
```

> 源代码：[step5](https://gitee.com/duycc/CMake-Tutorial/tree/main/step5)

### Step6 添加自定义命令生成文件

假设我们不打算使用系统提供的log和exp函数，而是打算在mysqrt函数中预先生成一系列的值。我们将在构建的过程中创建一个表并且将这个表编译到我们的程序中。首先移除log和exp的检测函数和mysqrt.cxx中的`HAVE_LOG`和  `HAVE_EXP` 的检测。在MathFunctions目录中，新建一个文件 `MakeTable.cxx` ：

```cpp
// A simple program that builds a sqrt table
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int i;
  double result;

  // make sure we have enough arguments
  if (argc < 2) {
    return 1;
  }

  // open the output file
  FILE *fout = fopen(argv[1], "w");
  if (!fout) {
    return 1;
  }

  // create a source file with a table of square roots
  fprintf(fout, "double sqrtTable[] = {\n");
  for (i = 0; i < 10; ++i) {
    result = sqrt(static_cast<double>(i));
    fprintf(fout, "%g,\n", result);
  }

  // close the table with a zero
  fprintf(fout, "0};\n");
  fclose(fout);
  return 0;
}
```

该表是作为有效的C++代码生成的，输出的文件名称是以参数形式传入的。下一步是将合适的命令添加到MathFunctions中的CMakeLists.txt文件中来构建MakeTable的可执行文件，然后将其作为构建过程的一部分运行。更改该文件如下：

```cmake
add_library(MathFunctions 
            mysqrt.cxx
            ${CMAKE_CURRENT_BINARY_DIR}/Table.h
            )

target_include_directories(MathFunctions 
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
    )

add_executable(MakeTable MakeTable.cxx)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Table.h
    COMMAND MakeTable ${CMAKE_CURRENT_BINARY_DIR}/Table.h
    DEPENDS MakeTable
    )

install(TARGETS MathFunctions DESTINATION lib)
install(FILES MathFunctions.h DESTINATION include)
```

修改`mysqrt.cxx`文件包含头文件`Table.h`。重写`mysqrt`函数：

```cpp
#include <iostream>

#include "MathFunctions.h"
#include "Table.h"

double mysqrt(double x) {
  if (x <= 0) {
    return 0;
  }

  // use the table to help find an initial value
  double result = x;
  if (x >= 1 && x < 10) {
    std::cout << "Use the table to help find an initial value " << std::endl;
    result = sqrtTable[static_cast<int>(x)];
  }

  // do ten iterations
  for (int i = 0; i < 10; ++i) {
    if (result <= 0) {
      result = 0.1;
    }
    double delta = x - (result * result);
    result = result + 0.5 * delta / result;
    std::cout << "Computing sqrt of " << x << " to be " << result << std::endl;
  }

  return result;
}
```

当执行构建时，首先会构建`MakeTable`程序，然后运行`MakeTable`程序生成`Table.h`文件。最后编译`mysqrt.cxx`文件并生成`MathFunctions`库。运行`Tutorial`可执行程序：

```bash
duyong@Mac: build (main) $ ./Tutorial 6
Use the table to help find an initial value 
Computing sqrt of 6 to be 2.44949
# ...
```

生成的`build/MathFunctions/Table.h`文件如下：

```cpp
double sqrtTable[] = {
0,
1,
1.41421,
1.73205,
2,
2.23607,
2.44949,
2.64575,
2.82843,
3,
0};
```

> 源文件：[step6](https://gitee.com/duycc/CMake-Tutorial/tree/main/step6)

### Step7 CPack生成安装包

假如我们要将程序分发给别人，别人可以安装并使用它。我们希望可以提供各个平台上都可以使用的二进制和源文件。这不同于我们之前做的[`Installing and Testing`](https://cmake.org/cmake/help/v3.21/guide/tutorial/Installing and Testing.html#guide:tutorial/Installing and Testing)，之前我们安装的文件是我们自己从源代码构建的可执行陈序。现在我们要构建一个支持二进制安装的安装包。我们将使用CPack创建特定平台的安装器。修改根目录下的CMakeLists.txt，添加如下行：

```cmake
include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${Tutorial_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${Tutorial_VERSION_MINOR}")
include(CPack)
```

[`InstallRequiredSystemLibraries`](https://cmake.org/cmake/help/v3.21/module/InstallRequiredSystemLibraries.html#module:InstallRequiredSystemLibraries)这个模块将包含此项目在当前平台上需要的所有运行时库。然后我们设置了一些CPack变量存储证书和版本号。最后包含了 [`CPack module`](https://cmake.org/cmake/help/v3.21/module/CPack.html#module:CPack) 模块，此模块将使用这些属性和其它属性生成一个安装器。构建项目查看生成的文件：

```bash
duyong@Mac: build (main) $ tree -L 1
.
├── CMakeCache.txt
├── CMakeFiles
├── CPackConfig.cmake
├── CPackSourceConfig.cmake
├── CTestTestfile.cmake
├── Makefile
├── MathFunctions
├── TutorialConfig.h
└── cmake_install.cmake

2 directories, 7 files
```

注意生成的CPackConfig.cmake和CPackSourceConfig.cmake文件。

按照一般的方式构建项目，然后运行Cpack。要构建一个二进制版本，可以运行`cpack --config CPackConfig.cmake`，输出日志如下：

```
duyong@Mac: build (main) $ cpack --config CPackConfig.cmake 
CPack: Create package using STGZ
CPack: Install projects
CPack: - Run preinstall target for: Tutorial
CPack: - Install project: Tutorial []
CPack: Create package
CPack: - package: /Users/duyong/Study/CMake-Tutorial/build/Tutorial-1.0-Darwin.sh generated.
CPack: Create package using TGZ
CPack: Install projects
CPack: - Run preinstall target for: Tutorial
CPack: - Install project: Tutorial []
CPack: Create package
CPack: - package: /Users/duyong/Study/CMake-Tutorial/build/Tutorial-1.0-Darwin.tar.gz generated.
```

从日志可以看出生成了两个打包的文件`Tutorial-1.0-Darwin.sh`和`Tutorial-1.0-Darwin.tar.gz`：

```bash
duyong@Mac: build (main) $ tree -L 1
.
├── CMakeCache.txt
├── CMakeFiles
├── CPackConfig.cmake
├── CPackSourceConfig.cmake
├── CTestTestfile.cmake
├── Makefile
├── MathFunctions
├── Tutorial
├── Tutorial-1.0-Darwin.sh
├── Tutorial-1.0-Darwin.tar.gz
├── TutorialConfig.h
├── _CPack_Packages
├── cmake_install.cmake
└── install_manifest.txt

3 directories, 11 files
```

查看压缩包内的文件：

```bash
duyong@Mac: build (main) $ tar -tf Tutorial-1.0-Darwin.tar.gz 
Tutorial-1.0-Darwin/bin/
Tutorial-1.0-Darwin/bin/Tutorial
Tutorial-1.0-Darwin/include/
Tutorial-1.0-Darwin/include/MathFunctions.h
Tutorial-1.0-Darwin/include/TutorialConfig.h
Tutorial-1.0-Darwin/lib/
Tutorial-1.0-Darwin/lib/libMathFunctions.a
```

可以看到这个是已经编译好的压缩包，解压后可直接运行。要构建源代码版本，可运行：

```cmake
cpack --config CPackSourceConfig.cmake
```

> 源代码：[step7](https://gitee.com/duycc/CMake-Tutorial/tree/main/step7)



***

### CMake资料：

* [CMake 入门实战](https://www.hahack.com/codes/cmake/)
* [CMake实践.pdf](http://wiki.ustchcs.com:8080/wiki/lib/exe/fetch.php?media=progress:duyong:cmake_practice.pdf)

***


