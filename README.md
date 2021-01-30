# MicroML

Quick-start machine learning projects on microcontrollers with help of [TensorFlow Lite for Microcontrollers](https://github.com/tensorflow/tensorflow/tree/master/tensorflow/lite/micro) and [libopencm3](https://github.com/libopencm3/libopencm3)

## Content

- [Introduction](#Introduction)
- [Prerequisites](#Prerequisites)
- [Installation and setup](#Installation-and-setup)
- [Building your projects](#Building-your-projects)
- [Blinky and uart example](#Blinky-and-uart-example)
- [Hello world example](#Hello-world-example)
- [Cifar example](#Cifar-example)
- [Why I created MicroML](#Why-I-created-MicroML)

## <a name="Introduction"></a> Introduction

*\"The future of machine learning is tiny\" - Pete Warden* 
 
As the tech lead of TensorFlow Mobile Team said, microcontrollers will become increasingly important for machine learning applications. It seems that we are not far from the future where ML applications will be running on very small devices that require only batteries to work for months, even years. There are already a few tools and frameworks that enable engineers to flash pre-trained neural networks onto microcontrollers and execute them. 

[TensorFlow Lite for Microcontrollers](https://github.com/tensorflow/tensorflow/tree/master/tensorflow/lite/micro) is one of these. You create a pre-trained neural network on your host machine with help of TensorFlow Lite, quantize it and then flash it to your target platform. It already supports few microcontrollers out of the box such as Appolo3, Arduino Nano 33 BLE Sense, Arduino MKRZERO and few others.
Developers have also created few example projects that showcase its capabilities. 

In next chapters I will guide you through the whole process of setting up the MicroML repo. I will explain how the whole system works and how to configure it for your specific platform. 


## <a name="Prerequisites"></a> Prerequisites

### Windows

I suggest you to create an Ubuntu server with Oracle VM VirtualBox, then create a shared folder between a guest and host machine. That way you can change the code with an editor of your choice and then switch to the Ubuntu server to invoke make commands. There is a great tutorial on [youtube](https://www.youtube.com/watch?v=kYEzEFH5LiM) that guides you through the process while explaining the basics. I use exact same setup and it works as expected.  

I wrote down all necessary steps in [VirtualBox_setup.md](https://github.com/SkobecSlo/MicroML/blob/master/VirtualBox_setup.md) file. They are almost the same as in the video, with minor differences.

I tried using TensorFlow Lite on Windows with help of Cygwin and Msys. This setup did not work as I was getting weird errors while trying to build a simple `hello world` example. 

### Linux 

No special setup is needed, you might need to install tools that are listed under Ubuntu dev tools header in [VirtualBox_setup.md](https://github.com/SkobecSlo/MicroML/blob/master/VirtualBox_setup.md). 

### MacOS

Did not test my setup on any Mac machine, but it should not be that much different from Linux. If everything else fails on a Mac, there is still an option to use VM VirtualBox. 


## <a name="Installation-and-setup"></a> Installation and setup

### Clone repository

Run the command below to clone the MicroML project and to initialize and update submodules:

`git clone --recurse-submodules https://github.com/SkobecSlo/MicroML.git`

If you accidentally did only `git clone` then just move into MicroML directory and execute:

`git submodule update --init --recursive`

### TensorFlow setup 

Before we can even compile a simple example for our target, we need to run a `hello_world` example for a SparkFun Edge board.
This is needed because makefiles written by TensorFlow team pull in several external dependencies from third parties, which are missing by default. 
After compilation these libraries can be found under `tensorflow/lite/micro/tools/make/downloads`. 

To get them we first move inside `tensorflow` folder:

`cd tensorflow`

And then run:

`sudo make -f tensorflow/lite/micro/tools/make/Makefile TARGET=sparkfun_edge hello_world_bin`

This will call several scripts that will download the libraries, after this 
it will start compiling the source files. It might take a while, but we have to do this step only once.
It can happen that because of a slow internet connection not all libraries get downloaded and then you get a compiler error (such as missing `flatbuffer` header or similiar).
To try again you **need** to delete the generated files and downloaded libraries, and try again.

`sudo make -f tensorflow/lite/micro/tools/make/Makefile TARGET=sparkfun_edge hello_world_bin`
`rm -fr tensorflow/lite/micro/tools/make/downloads`

### Optional hello_world on host computer

This step is completely optional, you can run a `hello_world` example on your host system, by running below two commands:

```
sudo make -f tensorflow/lite/micro/tools/make/Makefile hello_world
./tensorflow/lite/micro/tools/make/gen/linux_x86_64/bin/hello_world
```

Quick explanation of TensorFlow `hello_world` example: it is feeding values into a model that models the sine wave function.
Both the input and output values are continuously printed.


### libopencm3 setup

libopencm3 will provide us with libraries for our targets, it will also generate the necessary linker and startup files. Visit the libopencm3 GitHub page [libopencm3](https://github.om/libopencm3/libopencm3) and make sure that your target is supported.

To generate all necessary files just run following command inside MicroML's root directory:

`make -C libopencm3`

## <a name="Building-your-projects"></a> Building your projects

### Tensorflow specific commands

If your project is using TensorFlow you need to run this command once before using general commands:

`make -f archive_makefile PROJECT=<name of our project>`

You also need need make sure that line `LIBDEPS = microlite_build/microlite.a` is inside `project.mk` file.

If you want to be able to run and test TensorFlow functions on your host machine without any microcontroller target, you need to run this command before using general test commands:

`make -f archive_makefile test PROJECT=<name of our project>`

Source files that are used only for testing purposes should be added to `TESTFILES` variable and filtered out from normal source files in `project.mk` file. An example of this can be seen inside `cifar_stm32f7/project.mk`.

### General commands

To build a project you can just run `make` inside the project.

To build and flash the target use `make flash`.

To build, flash and open minicom use `make monitor`.

To delete generated files use `make clean`.

To delete generated files including `microlite.a` use `make clean_all`.

### General test commands

To build and run a project you can just run `make test` inside the project.

To delete generated files use `make clean_test`.

To delete generated files including `testlite.a` use `make clean_test_all`.

### Project structure

Each project should contain all files inside one folder, that folder has to reside inside `projects` folder.

Each project folder should contain:
* Makefile, which we will call with make command
* project.mk, where we specify our MCU, files that we want to compile, test files
* openocd.cfg, which tells openocd how to flash firmware to our MCU
* source files

Project.mk and openocd.cfg are only two files where we have to change something specific to our project settings. In project.mk you specify the name of the MCU, in openocd.cfg you specify the correct programmer and correct target.

It is expected by default that your source files will be inside project folder itself.
This can get messy as number of source files increases, so it is possible to move source files into separate folders inside that specific project folder. 
In that case you have to change one line inside `project.mk` file, you can find instruction how to do that inside.

## <a name="Blinky-and-uart-example"></a> Blinky and uart example

Start with this example only after libopencm3 setup, TensorFlow is not needed here.


In `project` folder there are blinky and uart examples for two different MCUs that were available to me. 
These are the two most simple examples that you can test on your devices to see how the library works.
**They do not need Tensorflow library at all**, so they are perfect for testing new development boards.
They are almost exact copy of what you can find in libopencm3 examples repository.
Uart examples use mpaland's excellent printf library that can be found on [GitHub](https://github.com/mpaland/printf).

Of course some changes in the code will be needed to accommodate for a different pinout and/or architecture.

## <a name="Hello-world-example"></a> Hello world example

Start with this example only after you completed TensorFlow setup and libopencm3 setup.

This `hello world` example is exactly the same as `hello world` example mentioned in TensorFlow setup, but his time we will run it on a stm32f767zi microcontroller.

Because this example needs access to TensorFlow functions we first need to create a microlite.a archive file. We do this by moving into the main MicroMl directory and running:

`make -C tensorflow/ -f ../archive_makefile PROJECT=hello_world_stm32f7`

This will create a microlite_build folder in `hello_world_stm32f7` folder.

If you open  `hello_world_stm32f7/project.mk` you will notice an extra line which was not present in blinky and uart examples:

`LDLIBS = microlite_build/microlite.a`

This is needed to link microlite.a file in linking process.

Commands for building and flashing are exactly the same as before, if you run `make monitor` you should see x and y values scrolling by.

Congrats! You just ran your first neural net on a microcontroller!


## <a name="Cifar-example"></a> CIFAR example

Start with this example only after you completed TensorFlow setup and libopencm3 setup.

This example runs a simple, 3 classes, image classification model that was trained on CIFAR dataset. 
It must be mentioned that the point of this example was not to create a very accurate model, but to ensure that I can create a simple model in Google Colab, convert it, quantize it and then run it on a microcontroller.

The example loads CIFAR model and runs inference on six pictures. It outputs class probability for each picture and how long did inference take.

If you want to see program in action but you do not have a microcontroller near you, you have to run this in root directory of MicroML:

`make -C tensorflow/ -f ../archive_makefile test PROJECT=cifar_stm32f7`

And then this inside project folder:

`make test`

If you want to run example on a microcontroller you have to run this in root directory of MicroML:

`make -C tensorflow/ -f ../archive_makefile PROJECT=cifar_stm32f7`

And then this inside project folder:

`make monitor`

## <a name="Why-I-created-MicroML"></a> Why I created MicroML

As I wanted to create a ML application on microcontoller for my masters thesis, I decided to dive into TensorFlow and tried to make it work for my particular platform. I ran into a few problems while doing this, some of them stemmed from my lack of experience, others from the way TensorFlow was set to be used.

Problems:
- TensorFlow expects you to create your project inside their examples directory (at least that seemed the easiest way). This example directory is buried under many other directories, which was definitely something that I did not like. I wanted my application code and TensorFlow to be separate.
- You build examples with help of 300 plus lines long main makefile, which calls several other smaller makefiles and scripts to download dependencies. It was very hard to understand fully what does what and what to change to achieve desired outcome.
- Target specific included makefiles and source code would have to be located on completely different places, which is harder to maintain and distribute.
- Sad reality is that embedded programmers (at least younger ones) are used to IDE's like CodeComposer, Keil and PlatformIO, that generate all target specific code (startup code, linker scripts, compiler and linker flags) for you, so you can focus on application code. This means that porting TensorFlow to your target architecure is even harder, as you usually do not know all required compiler flags, startup and linker files that are needed.

Because of above mentioned problems I started working on a solution that became MicroML.
I wanted to keep project specific code and TensorFlow separated.
I wanted some sort of system which will provide me with all target specific code, just by me providing the name of the platform to it.
This would enable me to quickly switch platforms and test how different microcontrollers are executing the same ML model.
I did not want to use any IDE, a simple text editor such as Vim or Sublime Text should be sufficient.
I did not want to use some bloated, over-engineered firmware library such as ST's HAL, that would be too complicated to use and change.

While researching I stumbled upon a very good project, [libopencm3](https://github.om/libopencm3/libopencm3).
The libopencm3 project aims to create an open-source firmware library for various ARM Cortex-M microcontrollers.
It supports a variety of different ARM microcontrollers and needs from user only the microcontroller's name tag to generate specific firmware libraries, startup code, linker scripts and compiler flags.

MicroML combines Tensorflow Lite and libopencm3. The developer first builds each module separately, then creates a microlite.a archive file for it's specific project. Then it compiles it's project code and links it against microlite.a file. The final product are executable files which can be flashed on desired platform with help of programmer of your choice (combination of OpenOCD and ST-Link V2 did the job for me). 
