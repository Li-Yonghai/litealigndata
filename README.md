# LiteAlignData

LiteAlignData is a C++ data generation framework designed for generating test data. This project is primarily intended for network protocol data analysis scenarios and supports the generation and processing of multiple data formats.

## Project Overview

LiteAlignData is a lightweight data alignment and generation tool with the following key features:

- **Multi-format Data Support**: Supports parsing and generation of multiple data formats, including Ctrldata, Example, and others
- **Flexible Data Processing**: Provides various data processing mechanisms such as random functions, accumulator functions, and unique random functions
- **Network Streaming Service**: A high-performance network server based on libevent, supporting streaming data output
- **XML Configuration**: Defines data structures via XML configuration files, supporting complex table relationships and dependencies
- **Dimension File Support**: Supports reading data from dimension files and performing mapping operations

## Directory Structure

```
litealigndata/
├── src/                          # Source code directory
│   ├── com/                      # Common components
│   │   ├── AlignDataTypeDef.cc   # Data type definitions
│   │   ├── AlignDataUtils.cc     # Utility functions
│   │   └── Comm.cc               # Common communication and type mappings
│   ├── conf/                     # Configuration module
│   │   ├── AlignConfig.cc        # Configuration management
│   │   └── AligndataConfig.cc    # Data configuration parsing
│   ├── file/                     # File format handling
│   │   ├── BaseFileFormat.cc     # Base file format
│   │   ├── Ctrldata/             # Ctrldata format handling
│   │   └── Example/              # Example format handling
│   ├── fun/                      # Function handling
│   │   ├── BaseFuncClass.cc      # Base function class
│   │   └── FunctionObject.cc     # Function object handling
│   ├── log/                      # Logging module
│   │   └── LogHandler.cc         # Log handling
│   ├── stream/                   # Network streaming service
│   │   ├── Acceptor.cc           # Connection acceptor
│   │   ├── Server.cpp            # Server main program
│   │   ├── ThreadManager.cc      # Thread management
│   │   └── WorkThread.cc         # Worker thread
│   ├── struct/                   # Data structures
│   │   ├── AlignDataObject.cc    # Data object base class
│   │   ├── ColumnObject.cc       # Column object
│   │   ├── TableObject.cc        # Table object
│   │   └── DataStructure.cc      # Data structure definitions
│   └── main.cc                   # Main program entry
├── testcase/                     # Test cases
│   ├── case/                     # Test scenarios
│   └── apimock/                  # API mocking
├── tools/                        # Utility tools
│   ├── Client/                   # Client tool
│   └── DeEncode/                 # Encoding/decoding tool
├── seedfile/                     # Seed files
│   ├── Ctrldata/                 # Control data
│   └── Example/                  # Example data
├── stacks/                       # Third-party dependency libraries
│   └── tarballs/                 # Dependency packages
├── DockerBuild/                  # Docker build configuration
├── DockerJenkins/                # Jenkins Docker configuration
├── build_all.sh                  # Full build script
├── build_src.sh                  # Source code build script
└── clean_all.sh                  # Cleanup script
```

## Core Features

### Data Type Support

The project supports a rich set of data types, including:
- Integer types: UINT64, INT64, UINT32, INT32, UINT16, INT16, UINT8, INT8
- Network types: IPv4, IPv6, IPv4V6, IPv4_NET, IPv6_NET
- String types: STRING, ByteArray
- Time types: DateTime, RealTime, RealTime_ms, TIMEVAL
- Special encodings: BCD, IMSIBCD, IMEISV_BCD, SP_U8_4, SP_IPV6

### Function Processing

- **RandomFunc**: Generates random data with multiple randomization modes
- **AcculateFunc**: Accumulation calculation function
- **MergeFunc**: Data merging function
- **UniqueRandomFunc**: Generates unique random numbers
- **fileReaderFunc**: Reads data from files

### Network Service

A high-performance server implemented using libevent, supporting:
- TCP connection management
- Heartbeat detection
- Streaming data output
- Multi-threaded processing

## Build Requirements

- C++ compiler (GCC 7.0+)
- CMake 3.26+
- ACE 7.1.0
- libevent 2.1.12
- Xerces-C 3.1.4
- Boost 1.64.0
- Other dependencies (see stacks/tarballs/)

## Quick Start

### 1. Build the Project

see INSTALL pls.

### 2. Run the Program

see INSTALL pls.

### 3. Connect with Client

see INSTALL pls.

## Configuration Details

### XML Configuration File

The project uses XML files to define data structures, including:
- Table definitions (Table)
- Column definitions (Column)
- Function definitions (Function)
- Dependencies (Dependency)

## Docker Deployment

### Build the Image

```bash
cd DockerBuild
./build.sh
```

### Run Jenkins Service

```bash
cd DockerJenkins
docker-compose up
```

## Testing

The project includes a comprehensive test framework:

```bash
cd testcase/case
./casemain
```

## License

For detailed licensing information, please refer to the LICENSE file.

## Contributions

Issues and pull requests are welcome.