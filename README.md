# PTracer
## A command-line compiler listener for Windows 10
PTracer allows to extract compile database on windows as well as intercept process in windows. Literally any kind of application could be intercepted via  `PTracer`. 


```
PTracer is a command line application which observes any applications processes on Windows 10.
PTracer observe a process and its child process

Options

--help, -h                               help and options
--version                                see version
--verbose, -v                            verbose
--output, -o                             output as json

Usage:
PTrace [application]
```
Supported Compilers
```
* GCC/G++
* Clang/Clang++
* Go compiler
* MSBuild(as build system)
```
Does not support
```
* Convention based build systems Gradle and Maven(currently)
```
For example we can observe the compilation process simple c++ application 
main.cpp
```
#include<iostream>

int main()
{
  std::cout << "Hello, PTracer is watching you!\n";
}
```
```
PTracer g++ main.cpp -o hello
```
The JSON output
```
{
  "directory": "C:/Users/mehme/Desktop/PTracer/x64/Release",
  "Command": "make",
  "Children": [
    {
      "directory": "C:/Users/mehme/Desktop/PTracer/x64/Release",
      "Command": "g++ -Wall -o hello main.cpp",
      "Children": [
        {
          "directory": "C:/Users/mehme/Desktop/PTracer/x64/Release",
          "Command": "'C:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../libexec/gcc/i686-w64-mingw32/8.1.0/cc1plus.exe' -quiet -iprefix 'C:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../lib/gcc/i686-w64-mingw32/8.1.0/' -D_REENTRANT main.cpp -quiet -dumpbase main.cpp -mtune=generic -march=i686 -auxbase main -Wall -o C:/Users/mehme/AppData/Local/Temp/ccGxXmkS.s",
          "Children": []
        },
        {
          "directory": "C:/Users/mehme/Desktop/PTracer/x64/Release",
          "Command": "'C:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../lib/gcc/i686-w64-mingw32/8.1.0/../../../../i686-w64-mingw32/bin/as.exe' -o C:/Users/mehme/AppData/Local/Temp/ccGsCIr2.o C:/Users/mehme/AppData/Local/Temp/ccGxXmkS.s",
          "Children": []
        },
        {
          "directory": "C:/Users/mehme/Desktop/PTracer/x64/Release",
          "Command": "'C:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../libexec/gcc/i686-w64-mingw32/8.1.0/collect2.exe' -plugin 'C:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../libexec/gcc/i686-w64-mingw32/8.1.0/liblto_plugin-0.dll' '-plugin-opt=C:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../libexec/gcc/i686-w64-mingw32/8.1.0/lto-wrapper.exe' -plugin-opt=-fresolution=C:/Users/mehme/AppData/Local/Temp/ccB04OV3.res -plugin-opt=-pass-through=-lmingw32 -plugin-opt=-pass-through=-lgcc_s -plugin-opt=-pass-through=-lgcc -plugin-opt=-pass-through=-lmoldname -plugin-opt=-pass-through=-lmingwex -plugin-opt=-pass-through=-lmsvcrt -plugin-opt=-pass-through=-lpthread -plugin-opt=-pass-through=-ladvapi32 -plugin-opt=-pass-through=-lshell32 -plugin-opt=-pass-through=-luser32 -plugin-opt=-pass-through=-lkernel32 -plugin-opt=-pass-through=-liconv -plugin-opt=-pass-through=-lmingw32 -plugin-opt=-pass-through=-lgcc_s -plugin-opt=-pass-through=-lgcc -plugin-opt=-pass-through=-lmoldname -plugin-opt=-pass-through=-lmingwex -plugin-opt=-pass-through=-lmsvcrt --sysroot=C:/mingw810/i686-810-posix-dwarf-rt_v6-rev0/mingw32 -m i386pe -Bdynamic -u ___register_frame_info -u ___deregister_frame_info -o hello.exe 'C:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../lib/gcc/i686-w64-mingw32/8.1.0/../../../../i686-w64-mingw32/lib/../lib/crt2.o' 'C:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../lib/gcc/i686-w64-mingw32/8.1.0/crtbegin.o' '-LC:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../lib/gcc/i686-w64-mingw32/8.1.0' '-LC:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../lib/gcc' '-LC:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../lib/gcc/i686-w64-mingw32/8.1.0/../../../../i686-w64-mingw32/lib/../lib' '-LC:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../lib/gcc/i686-w64-mingw32/8.1.0/../../../../lib' '-LC:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../lib/gcc/i686-w64-mingw32/8.1.0/../../../../i686-w64-mingw32/lib' '-LC:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../lib/gcc/i686-w64-mingw32/8.1.0/../../..' C:/Users/mehme/AppData/Local/Temp/ccGsCIr2.o -lstdc++ -lmingw32 -lgcc_s -lgcc -lmoldname -lmingwex -lmsvcrt -lpthread -ladvapi32 -lshell32 -luser32 -lkernel32 -liconv -lmingw32 -lgcc_s -lgcc -lmoldname -lmingwex -lmsvcrt 'C:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../lib/gcc/i686-w64-mingw32/8.1.0/crtend.o'",
          "Children": [
            {
              "directory": "C:/Users/mehme/Desktop/PTracer/x64/Release",
              "Command": "'C:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../lib/gcc/i686-w64-mingw32/8.1.0/../../../../i686-w64-mingw32/bin/ld.exe' -plugin 'C:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../libexec/gcc/i686-w64-mingw32/8.1.0/liblto_plugin-0.dll' '-plugin-opt=C:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../libexec/gcc/i686-w64-mingw32/8.1.0/lto-wrapper.exe' -plugin-opt=-fresolution=C:/Users/mehme/AppData/Local/Temp/ccB04OV3.res -plugin-opt=-pass-through=-lmingw32 -plugin-opt=-pass-through=-lgcc_s -plugin-opt=-pass-through=-lgcc -plugin-opt=-pass-through=-lmoldname -plugin-opt=-pass-through=-lmingwex -plugin-opt=-pass-through=-lmsvcrt -plugin-opt=-pass-through=-lpthread -plugin-opt=-pass-through=-ladvapi32 -plugin-opt=-pass-through=-lshell32 -plugin-opt=-pass-through=-luser32 -plugin-opt=-pass-through=-lkernel32 -plugin-opt=-pass-through=-liconv -plugin-opt=-pass-through=-lmingw32 -plugin-opt=-pass-through=-lgcc_s -plugin-opt=-pass-through=-lgcc -plugin-opt=-pass-through=-lmoldname -plugin-opt=-pass-through=-lmingwex -plugin-opt=-pass-through=-lmsvcrt --sysroot=C:/mingw810/i686-810-posix-dwarf-rt_v6-rev0/mingw32 -m i386pe -Bdynamic -u ___register_frame_info -u ___deregister_frame_info -o hello.exe 'C:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../lib/gcc/i686-w64-mingw32/8.1.0/../../../../i686-w64-mingw32/lib/../lib/crt2.o' 'C:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../lib/gcc/i686-w64-mingw32/8.1.0/crtbegin.o' '-LC:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../lib/gcc/i686-w64-mingw32/8.1.0' '-LC:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../lib/gcc' '-LC:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../lib/gcc/i686-w64-mingw32/8.1.0/../../../../i686-w64-mingw32/lib/../lib' '-LC:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../lib/gcc/i686-w64-mingw32/8.1.0/../../../../lib' '-LC:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../lib/gcc/i686-w64-mingw32/8.1.0/../../../../i686-w64-mingw32/lib' '-LC:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../lib/gcc/i686-w64-mingw32/8.1.0/../../..' C:/Users/mehme/AppData/Local/Temp/ccGsCIr2.o -lstdc++ -lmingw32 -lgcc_s -lgcc -lmoldname -lmingwex -lmsvcrt -lpthread -ladvapi32 -lshell32 -luser32 -lkernel32 -liconv -lmingw32 -lgcc_s -lgcc -lmoldname -lmingwex -lmsvcrt 'C:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/../lib/gcc/i686-w64-mingw32/8.1.0/crtend.o'",
              "Children": []
            }
          ]
        }
      ]
    }
  ]
}
```
Set custom compiler on windows 
```
PTracer.exe  cmake -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang .
```
#### Notes
To use extracted batch script, remove the first command from `compile.bat`
```
#### Compile from source code
* Visual Studio 19
* Windows Operating System(tested on Windows 10)
```
#### Credits
Many thanks to Roger Or for his aweseome tool [NtTrace](https://github.com/rogerorr/NtTrace)
