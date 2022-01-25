## Oled test project.

This repository contains source code and self contained build system for the given device.

## Motivation.

I wanted to try out the [**OLED**][1] screen I got from china's manufacturer.
The target hardware device used for it is **NUCLEO-F756ZG** board with the corresponding MCU.

## Directory structure.

* [**./doc**][2] - contains all the documentation regarding the test board, screen and some videos.

* [**./prog**][3] - contains the *main.cpp* of the project.

* [**./ext**][4] - contains source code from web, like [**STM32CubeF7**][5] code package.

* [**./modules**][6] - contains code and drivers for the target MCU.

* **./out** - by default is a folder where everything gets built.

## Docker image. (Linux)

One can also use a [**docker**][7] image that is preconfigured and ready to go.

To build the docker image from [**Dockerfile**][8]: (assuming we are inside the project's folder)

`docker image build -t koto_doc .`

To run the docker container:

`docker run -it -v ${PWD}:/home/koto/Oled --privileged koto_doc`

**[NOTE]** ${PWD} forwards current folder path, can be replaced with other
project path.

To compile and flash from within the docker container:

`cd /home/koto/Oled/out/ && rm -rf * && cmake .. &&  make -j run`

**[NOTE]** For the **Flash Programmer** to work, plug the board in before running the docker image.

To just compile the code, use:

`cd ./Oled/out && cmake .. && make -j`

To generate the code documentation with call-graphs, use: `make doc`

`cd ./Oled/out && cmake .. && make -j doc`

**[NOTE]** The software for the **Flash Programmer** from the target board is installed while building Docker image.

[1]: ./doc/OledScreen_PV13904PY24G.pdf
[2]: ./doc
[3]: ./prog
[4]: ./ext
[5]: https://github.com/STMicroelectronics/STM32CubeF7
[6]: ./modules
[7]: https://www.docker.com/resources/what-container
[8]: ./Dockerfile
