# Life Application Suite

### About
I created this application to streamline development of useful everyday tools to track aspects of my life as well as learn more about software development. Any input or help is always apprecitated and I look forward to learning more.

##### Technical Details
The utilization of the ImGui library means creation of GUI tools is simple and highly customizable. The LAS Environment contains a log viewer and a limited shell to interact with the LAS framework itself and user-created Modules. RC files similar to the `.bashrc` file are used to provide customizability in a simple and familiar format.

### Setup
Prerequisites:
- Requires CMake
- Currently **only Linux is supported**

#### Setup of Build Environment:
This initial setup **must** be done before install for either the release or debug versions.
1. Clone the project
```
git clone <URL>
```
2. Navigate into the `LAS` directory
```
cd LAS
```
3. Run the setup script to create necessary directories for CMake.
 
*(The script creates a `bin/` directory and subseuquent `Debug` and `Release` directories for the LAS Environment and SDK)*
```
./build/scripts/setup-build-env.sh
```
#### Building the Project
 The commands to build each target do not change for the `Release` and `Debug` versions. The output will depend on the CMake `DEBUG` flag being set (see the [following section](#Set-Debug-Flag-For-Development) for more information). 
 
 If you wish to change to `DEBUG` mode, you must [set the flag](#Set-Debug-Flag-For-Development) before building each target.

*NOTE: The LAS-SDK must always be built first since the compiling of the LAS Environment requires the LAS-SDK library.*

*NOTE: It is recommended to contain build files in the `build/` directory for cleanliness*

Build the LAS-SDK 
```
cmake --build <path to top-level CMakeLists.txt> --target LAS-SDK
```
Build the LAS Environment 
```
cmake --build <path to top-level CMakeLists.txt> --target LAS
```
#### Set Debug Flag for Development
This section is *optional*. When the following variable is `ON`, certain functionality is added that aids in debugging and development work.
```
cmake -DDEBUG=ON <path to top-level CMakeLists.txt>
```
Turn off this variable for to get `Release` functionality
```
cmake -DDEBUG=OFF <path to top-level CMakeLists.txt>
```

### License

The LAS is licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3, see [LICENSE](LICENSE) for more information.

Third-party libraries are licensed under their respective licenses.
 - ImGui uses the MIT License, see [imgui License.txt](Third-Party%20Licenses/imgui%20License.txt) for more information
 - The C++ JSON library uses the MIT License, see [json License.txt](Third-Party%20Licenses/json%20License.txt) for more information
