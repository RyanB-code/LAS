
# The Life Application Suite (LAS)


## Purpose of the Project

This project is a way to provide a faster way to develop GUI applications that generally follow my desired way of interacting with a computer: having GUI elements for pretty presentation of information, and a command line interface for automation and getting behind the curtains to tailor the experience to what I want.

***

## What This Site Is

This site will document the Module API in its entierty. Going in depth in the innerworkings of the Environment is not my intent, however thorough explanations will be provided that allows the developer to understand how the API is used during runtime. 

Proper setup of the Environment and what happens when a Module is loaded will also be explained to understand how to best create a Module.

***

## Parts of the LAS

The Life Application Suite (LAS) comes with two main parts: the Environment and the Module API (SDK).

### Environment

The Environment is the core of the application that can be thought of as the operating system. It handles application setup, loading and unloading of user-created Modules, the logging system, command execution, and the basic GUI.

The version of the Environment is independent of the module API (called the LAS SDK). This means that compatibility of a Module should be cross checked: one side being the SDK version that the Module was compiled with, and the other being the Environment's SDK version that it was compiled with.

***NOTE:*** The log viewer will display version mismatches if found when loading a module.

### Module API (SDK)

The Module API (referred to herin as the SDK) is used to create a Module to use in the Environment. A Module's lifetime and status is controlled by the Environment. 


This site holds documentation on the Life Application Suite's (LAS) module API referred to as the 'LAS SDK' (or just SDK). The SDK's version is independent to the LAS Environment.

The SDK provides access to the following functionality:

- A command interface to allow automation for the power-user
- Central logging library unified with the rest of the Environment
- A quick and easy way to save information to disk (using [Niel Lohmann's JSON library](https://json.nlohmann.me/) )
- A GUI using the popular [ImGUI library](https://github.com/ocornut/imgui)

