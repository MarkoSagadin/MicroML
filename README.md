# MicroML

Quick-start machine learning projects on microcontrollers with help of [TensorFlow Lite for Microcontrollers](https://github.com/tensorflow/tensorflow/tree/master/tensorflow/lite/micro) and [libopencm3](https://github.com/libopencm3/libopencm3)

## Introduction

*\"The future of machine learning is tiny\" - Pete Warden* 
 
As the tech lead of TenosorFlow Mobile Team said, microcontrollers will become increasingly important for machine learning applications. It seems that we are not far from the future where ML applications will be running on very small devices that require only batteries to work for months, even years. There are already few tools and frameworks that enable engineers to flash pre-trained neural networks on microcontrollers and execute them. 

[TensorFlow Lite for Microcontrollers](https://github.com/tensorflow/tensorflow/tree/master/tensorflow/lite/micro) is one of them. You create a pre-trained neural network on your host machine with help of TensorFlow Lite, quantize it and then flash it to your target platform. It already supports few microcontrollers out of the box such as Appolo3, Arduino Nano 33 BLE Sense, Arduino MKRZERO and few others.
Developers also created few example projects that showcase its capibilities. 

In next chapters I will guide you through the whole process of setting up the MicroML repo. I will explain how the whole system works and how to configure it for your specific platform. 


## Why I created MicroML

As I wanted to create a ML application on microcontoller for my master thesis, I decided to dive into TensorFlow and tried to make it work for my particular platform. I ran into a few problems while doing this, some of them stemmed from my lack of experience, others from the way TensorFlow was set to be used.

Problems:
- TensorFlow expects you to create your project inside their examples directory (at least that seemed the easiest way). This example directory is buried under many other directories, which was definitely something that I did not like. I wanted my application code and TensorFlow to be separate.
- You build examples with help of 300 plus lines long main makefile, which calls several other smaller makefiles and scripts to download dependencies. It is very hard to understand fully what does what and what to change to achieve desired outcome.
- Target specific included makefiles and source code would have to be located on completely different places, which is harder to maintain and distribute.
- Sad reality is that embedded programmers (at least younger ones) are used to IDE's like CodeComposer, Keil and PlatformIO, that generate all target specific code (startup code, linker scripts, compiler and linker flags) for you, so you can focus on application code. This means that porting TensorFlow to your target architecure is even harder, as you usually do not know all required settings that are needed.

Because of above mentioned problems I started working on a solution that became MicroML. I wanted to keep project specific code and TensorFlow separated. I wanted some sort of system which will provide me with all target specific code, just by me providing the name of the platform to it. This would enable me to quickly switch platforms and test how different microcontrollers are executing the same ML model. I did not want to use any IDE, a simple text editor such as Vim or Sublime Text should be sufficient. I did not want to use some bloated, over-engineered firmware library such as ST's HAL, that would be too complicated to use and change.

While researching I stumbled upon a very good project, [libopencm3](https://github.com/libopencm3/libopencm3). The libopencm3 project aims to create an open-source firmware library for various ARM Cortex-M microcontrollers. It supports a variety of different ARM microcontrollers and needs from user only the microcontroller's name tag to generate specific firmware libraries, startup code, linker scripts and compiler flags.

MicroML combines Tensorflow Lite and libopencm3. Programmer first builds each module separately, then creates a microlite.a archive file for it's specific project. Then it compiles it's project code and links it against microlite.a file, final product are executable files which can be flashed on desired platform with help of programmer of your choice (combination of OpenOCD and ST-Link V2 did the job for me). 


## Prerequisites

I suggest you to create Ubuntu server with Oracle VM VirtualBox, create a shared folder between a guest and host machine. That way you can change the code with an editor of your choice and then switch to Ubuntu server to invoke make. There is a great tutorial on [youtube](https://www.youtube.com/watch?v=kYEzEFH5LiM) that guides you through the process while explaining the basics. I use exact same setup and it works as expected.  

I wrote down all neccessery steps in [VirtualBox_setup.md](https://github.com/SkobecSlo/MicroML/blob/master/VirtualBox_setup.md) file. They are almost the same as in the video, with minor differences. In the end I wrote steps for setting up st-link to flash programs to microcontorllers from guest machine.

I tried using TensorFlow Lite on Windows with help of Cygwin and Msys. This setup did not work as I was getting weird errors while building a simple hello world example. 

You can of course compile the project on Linux itself without VirtualBox, although some problems also appeared while building hello world example out of the box. These problems are easily solvable, refer to Troubleshooting section. 

## Installing

### Clone repository
Run below command to clone MicroML project and to initialize and update submodules:

`git clone --recurse-submodules https://github.com/SkobecSlo/MicroML.git`

If you accidentally did only git clone then just move into MicroML directory and execute:

`git submodule update --init --recursive`

### TensorFlow setup 
Before we can even compile a simple example for our target, we need to run a hello world
example that will run on our OS. This is needed because makefile written by 
TensorFlow team pulls in several necessary repositories from third parties, 
which by defualt are missing. After compilation these libraries can be found under 
`tensorflow/lite/micro/tools/make/downloads`. 

To get them we first do:

`cd tensorflow`

And then 

`sudo make -f tensorflow/lite/micro/tools/make/Makefile hello_world`

This will call several scripts that will download the libraries and then 
it will start compiling the source files. It might take a while.

Quick explanation on tensorflow hello_world example:
It is feeding values into a model that represents the sine wave function which calculates the output. Both the input and output values are continuously printed.
You can see program in action by running following command:

`./tensorflow/lite/micro/tools/make/gen/linux_x86_64/bin/hello_world`

### libopencm3 setup

Libopencm3 will provide us with libraries for out targets, it will also generate necessary linker and startup files.
To generate all necessary files just do inside root MicroML directory:

`make -C libopencm3`

### Hello World, sine wave example on STM32F405GT microcontroller

Start with this section only after you completed TensorFlow setup and libopencm3 setup.

Move into main MicroML directory and run:

`make -C tensorflow/ -f ../archive_makefile PROJECT=hello_world`

This will create microlite.a archive file which will be linked against in linking process.

Now compile and flash program to the microcontroller:

```
cd projects/hello_world/
make flash
```

Open your favorite serial terminal and watch values scrolling by.

# To generate microlite.a file for your particular project you have
  5 # to run the following command from main directory:
  6 # make -C tensorflow/ -f ../archive_makefile PROJECT=<name_of_your_project>
  7 # This will create microlite.a file for your target system, which will be
  8 # included in the linking process.
  9 # Variable PROJECT has to match to the name of your project folder inside
 10 # projects directory.

## Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.

## Authors

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

