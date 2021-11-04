SASM (SimpleASM) - простая кроссплатформенная среда разработки для языков ассемблера NASM, MASM, GAS, FASM с подсветкой синтаксиса и отладчиком. В SASM Вы можете легко разрабатывать и выполнять программы, написанные на языках ассемблера NASM, MASM, GAS, FASM. Вводите код в форму и запускайте приложение. Программа работает "из коробки" и хорошо подойдет для начинающих изучать язык ассемблера.
Основана на Qt. Распространяется по свободной лицензии GNU GPL v3.0.

SASM (SimpleASM) - simple Open Source crossplatform IDE for NASM, MASM, GAS, FASM assembly languages.
SASM has syntax highlighting and debugger. The program works out of the box and is great for beginners to learn assembly language. SASM is translated into Russian, English, Turkish (thanks Ali Goren), Chinese (thanks Ahmed Zetao Yang), German (thanks Sebastian Fischer), Italian (thanks Carlo Dapor), Polish (thanks Krzysztof Rossa), Hebrew (thanks Elian Kamal), Spanish (thanks Mariano Cordoba).
Licensed under the GNU GPL v3.0. Based on the Qt.



# How to build and run SASM:


## Linux

### Software package dependencies

  * For assembly of SASM programs themselves:
    * gcc (x86 OS) or gcc-multilib (x64 OS)
    * gdb
    * nasm
    * Ubuntu 21.10: $ sudo apt install nasm gdb gcc gcc-multilib

  * For QT5 building (recommended):
    * build-essential
    * qtbase5-dev
    * Ubuntu 21.10: $ sudo apt install build-essential qtbase5-dev

  * Compilation of SASM
    * $ export QT_SELECT=5 (or 4 for Qt4)
    * $ qmake
    * OR $ qmake PREFIX=<path>
    * $ make -j
    * Direct execution:
      * run "./sasm" from folder right away
    * Installation:
      * $ make install
      * Command "sasm" will open SASM



## Windows:

  * For building:
    * C++ compiler (e.g. gcc from MinGW)
    * make (e.g. mingw32-make from MinGW)
    * Qt 5

  * For running:
    * Everything needed is already included.

  * Instructions:
    * Download sources and unpack source.
    * Go to directory with their: "cd <path>"
    * 1) "qmake"
    * 2) "mingw32-make" for Windows.

  * For Windows:
    * Put "sasm.exe" executable file to folder "Windows". From this folder you can run the program.
    * Also you can run program right away from SASM folder.
    * If the program does not start after successful compilation and the error message (0xc000007b) appears, check that the path to GNU Compiler is set correctly in the environment variables.   




# Prebuild packages

Also you can download already compiled packages 
from site https://dman95.github.io/SASM/ or 
from OBS repository https://download.opensuse.org/repositories/home:/Dman95/

They might be outdated.

More help info in file help.html
Also SASM supports doxygen: run "doxygen configfile" to generate documentation. In this documentation you can also find a small developer guide which includes information about adding new assemblers and languages support.

Copyright © 2013 Dmitriy Manushin
