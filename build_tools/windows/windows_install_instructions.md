# Build and Setup Script for Hyperscan and Dependencies

This script automates the process of downloading, extracting, and building the Hyperscan library and its dependencies for a Windows environment using MSBuild.

## Features
- Configurable Visual Studio version and year (defaults to VS 17 and 2022).
- Downloads required third-party libraries (Boost, Ragel, PCRE, CMake, SQLite, Hyperscan).
- Builds and installs Ragel as a separate dependency.
- Compiles Hyperscan in both Debug and Release configurations.

## Prerequisites
Ensure the following utilities are installed and available in your environment:
- Python
- Curl
- Git
- Unzip
- Tar
- Make
- GCC

## Usage

1. **Run the Script**

   Execute the script with the following optional arguments:
   - `MSBuild Path`: Full path to the MSBuild executable.
   - `Visual Studio Version`: Version of Visual Studio (default: `17`).
   - `Visual Studio Year`: Year of Visual Studio (default: `2022`).

   Example:
   ```
   ./build_hyperscan.sh "/cygdrive/c/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/MSBuild/15.0/Bin" 17 2022
   ```

   If no arguments are provided, the script will prompt for the `MSBuild Path` and use default values for Visual Studio version and year.

2. **Verify MSBuild Path**

   Ensure the specified or entered path to `MSBuild.exe` is valid.

3. **Script Execution**

   The script performs the following steps:
   - Sets up the environment variables for the target platform and dependencies.
   - Downloads third-party libraries if they are not already present.
   - Extracts the downloaded archives.
   - Builds and installs the required dependencies.
   - Compiles Hyperscan for both Debug and Release configurations.

## Configuration
The script allows customization of the following:
- **Visual Studio Configuration**: Modify `DEFAULT_VS_VERSION` and `DEFAULT_VS_YEAR` variables to change the default Visual Studio settings.
- **Third-Party Library Versions**: Update the respective variables in the script (`BOOST_VER`, `RAGEL_VER`, etc.) to use different library versions.

## Output
- **Release Build**: Located in `lib/x64.Release/`.
- **Debug Build**: Located in `lib/x64.Debug/`.

## Example

To build Hyperscan with Visual Studio 2019 and a specific MSBuild path:
```
./build.sh "/cygdrive/c/Program Files (x86)/Microsoft Visual Studio/2019/BuildTools/MSBuild/15.0/Bin" 16 2019
```

## Notes
- This script is designed for use in a Cygwin environment on Windows.
- Ensure that all required tools are properly installed and accessible in the environment path.

## Troubleshooting
- If the script fails to find `MSBuild.exe`, verify the provided path.
- For missing utilities, install the required tools and rerun the script.
- Ensure network access for downloading third-party libraries.
