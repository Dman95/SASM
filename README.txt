﻿SASM (SimpleASM) - простая кроссплатформенная среда разработки для языков ассемблера NASM, MASM, GAS, FASM с подсветкой синтаксиса и отладчиком. В SASM Вы можете легко разрабатывать и выполнять программы, написанные на языках ассемблера NASM, MASM, GAS, FASM. Вводите код в форму и запускайте приложение. Программа работает "из коробки" и хорошо подойдет для начинающих изучать язык ассемблера.
Основана на Qt. Распространяется по свободной лицензии GNU GPL v3.0.

SASM (SimpleASM) - simple Open Source crossplatform IDE for NASM, MASM, GAS, FASM assembly languages.
SASM has syntax highlighting and debugger. The program works out of the box and is great for beginners to learn assembly language. SASM is translated into Russian, English, Turkish (thanks Ali Goren), Chinese (thanks Ahmed Zetao Yang), German (thanks Sebastian Fischer), Italian (thanks Carlo Dapor), Polish (thanks Krzysztof Rossa), Hebrew (thanks Elian Kamal), Spanish (thanks Mariano Cordoba).
Licensed under the GNU GPL v3.0. Based on the Qt.

===========================================================================
How to build and run SASM:
===========================================================================
You need:
On Windows:
For building:
    C++ compiler (e.g. gcc from MinGW)
    make (e.g. mingw32-make from MinGW)
    Qt 5
For running:
    Everything needed is included.

On Linux:
For building:
    build-essential
    qtbase5-dev
    qt5-default
For running:
    gcc-multilib (x64 OS) or gcc (x86 OS)
    gdb
    nasm

Download sources and unpack their.
Go to directory with their: "cd <path>"
Further print commands:
1) "qmake" (For installing in specific directory on Linux - print: "qmake PREFIX=<path>". By default SASM installs in "/usr/bin" and "usr/share")
2) "make" for Linux and "mingw32-make" for Windows.
3) For Linux: "make install" (command "sasm" will open SASM) or run "sasm" from folder right away or put "sasm" executable file to folder "Linux" (from this folder you can run the program).
   For Windows: Put "sasm.exe" executable file to folder "Windows". From this folder you can run the program. Also you can run program right away from compilation folder.
===========================================================================

Also you can download already compiled packages 
from site https://dman95.github.io/SASM/ or 
from OBS repository https://download.opensuse.org/repositories/home:/Dman95/

More help info in file help.html
Also SASM supports doxygen: run "doxygen configfile" to generate documentation. In this documentation you can also find a small developer guide which includes information about adding new assemblers and languages support.

Copyright © 2013 Dmitriy Manushin
