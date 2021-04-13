# ibmf-parser
Command-line parser for ISO/IEC base media files.

## Build (macOS / Linux)
Regular CMake procedure:

    mkdir _cmake && cd _cmake
    cmake ..
    make && ./ibmf-parser

or if you need an Xcode project (see CMake docs for other build options):

    cmake .. -GXcode
    open ibmf-parser.xcodeproj

Note that option ENABLE_TESTS is available:

    cmake .. -DENABLE_TESTS=ON

## Build (Windows)
Regular CMake procedure again:

    mkdir _cmake && cd _cmake
    cmake .. <-DENABLE_TESTS=ON>

Open ibmf-parser.sln with Visual Studio and set ibmf-parser as startup project.
