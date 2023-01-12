# How to Compile and Flash

## Toolchain
To compile the project we need a cross-compiler for ARM embedded systems (`arm-none-eabi`). It requires at minimum a version that supports C++17. You should, however, stick to the latest version if possible.

Depending of current Ubuntu version it is not recommended to 
install the toolchain via apt from the Ubuntu archives or a PPA, as these sources usually provide very outdated versions.
In that case it is better to download and install the toolchain manually:

1. Goto the [download site](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) and download the `arm-none-eabi` tarball for Linux hosts (for e.g. `arm-gnu-toolchain-x.x.x-x86_64-arm-none-eabi.tar.xz`)
2. Extract the files to the target install directory  of your choice, e.g. by `~/toolchains/`
3. Create symbolic links in `/usr/bin` to make our toolchain callable system wide by using the following command: `sudo ln -s  ~/toolchains/[your arm-none-eabi toolchain]/bin/arm-none-eabi-* /usr/bin`
4. Verify the output of `arm-none-eabi-gcc --version`

## Cmake

Without IDE the simplest approach to generate cmake files in `build` directory is to use this command in root of project:

```
cmake -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/arm-none-eabi-gcc -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/arm-none-eabi-g++ -B build
```
Then go into the build directory and now you can compile with `make`.

## VS Code 
Using an IDE will make your life easier. In this case the setup in VS Code will described here.

0. Install VS Code of course. (e.g. `sudo snap install code --classic`)
1. Install those extensions in VS Code:
    * CMake
    * CMake Tools
    * Cortex-Debug

2. Open the project directory `File > Open Folder...` 
3. Select kit with currently used toolchain. This should prompted by opening a list from top. If not you can select the kit in the bottom bar where `Not kit selected` is labeled.
4. Then you can hit the `Build` button in the bottom bar to compile.

## Flash 

It is recommend to do flashing with VS Code because it is simply and you can attach/debug to the running firmware after flashing.

For flashing we use J-Link debugger from Segger like [J-Link EDU Mini](https://www.segger.com/products/debug-probes/j-link/models/j-link-edu-mini/).
So at first we need to [download](https://www.segger.com/downloads/jlink/) the software from Segger for our J-Link. 



After we install J-Link software, compile the project and connect the target:

1.  go to Tab `Run and debug` on left
2. at the top right beside the play button you can select the debug profile. Select `Debug (J-Link, RTOS-aware)`. This ensures you target will be flashed with new firmware and after that you can debug with aware-ness of RTOS (like showing tasks list).
3. Hit F5 or press the play button

You firmware is flashed!

Note: If you want only attach to the running firmware without resetting them, select the debug profile `Attach (J-Link, RTOS-aware)`.
