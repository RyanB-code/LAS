# Life Application Suite

### About
I created this application to streamline development of useful everyday tools to track aspects of my life as well as learn more about software development. Any input or help is always apprecitated and I look forward to learning more.

##### Technical Details
The utilization of the ImGui library means creation of GUI tools is simple and highly customizable. The LAS Environment contains a log viewer and a limited shell to interact with the LAS framework itself and user-created Modules. RC files similar to the `.bashrc` file are used to provide customizability in a simple and familiar format.

### Setup
Prerequisites:
- Requires CMake
- Currently **only Linux is supported**

#### Setup of Build Environment
This initial setup **must** be done before building or installation of any portion of the project.
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
### Building the Project
- The initial setup **must** be done before building any portion of the project. See [previous section](#Setup-of-Build-Environment) for more information.
- The commands to build each target do not change for the `Release` and `Debug` versions. The output will depend on the CMake `DEBUG` flag being set (see the [following section](#Set-Debug-Flag-For-Development) for more information).
- If you wish to change to `DEBUG` mode, you must [set the flag](#Set-Debug-Flag-For-Development) before building each target.

*NOTE: The LAS-SDK must always be built first since the compiling of the LAS Environment requires the LAS-SDK library.*

*NOTE: It is recommended to contain build files in the `build/` directory for cleanliness*

#### Build the LAS-SDK 
```
cmake --build <dir> --target LAS-SDK
```
#### Build the LAS Environment 
```
cmake --build <dir> --target LAS
```
#### Set Debug Flag for Development
This section is *optional*. When the following variable is `ON`, certain functionality is added that aids in debugging and development work. Utilizing CMake's built in `--config Debug` option when building a target automatically sets this flag.
```
cmake -DDEBUG=ON <path to top-level CMakeLists.txt>
```
Turn off variable to get `Release` functionality.
```
cmake -DDEBUG=OFF <path to top-level CMakeLists.txt>
```

### Install
To install the project, you must first have built the `Release` version of both `LAS-SDK` and `LAS` itself. See [previous section](#Building-the-Project) for more information. Once this is complete, proceed with the following.

1. Run the install script from the top level directory. This copies the release soname version of the library to `/usr/local/lib/LAS`, and the `LAS.exe` to `/usr/local/bin`.
```
./build/scripts/install-release.sh
```
2. Set environment variable `LD_LIBRARY_PATH` automatically by amending the `.bashrc` file with the following. This allows the linker to know to look for libraries inside the specified directory.
```
LD_LIBRARY_PATH=/usr/local/lib/LAS
export LD_LIBRARY_PATH
```




### License

The LAS is licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3, see [LICENSE](LICENSE) for more information.

Third-party libraries are licensed under their respective licenses.
 - ImGui uses the MIT License, see [imgui License.txt](Third-Party%20Licenses/imgui%20License.txt) for more information
 - The C++ JSON library uses the MIT License, see [json License.txt](Third-Party%20Licenses/json%20License.txt) for more information
