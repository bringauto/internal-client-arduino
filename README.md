WIP

# Generating Protobuf files

> Generating InternalProtocol protobuf files is not necessary if InternalProtocol declaration in InternalProtocol.proto file is not changed.

From root directory go to source/protobuf and run this command:

```bash
python ../../libs/nanopb/generator/nanopb_generator.py InternalProtocol.proto
```

Now in current directory should be generated 2 new files: `InternalProtocol.pb.c` and `InternalProtocol.pb.h`. Include the header file to use InternalProtocol implementation. These generated files are moved in compiled directory. Some headers from nanopb submodule with universal function declarations are in source/nanopb directory.

# Generating zip library usable with Arduino IDE

In root directory use:

```bash
python zip_script
```

In current directory should be generated .zip, which can be used as internal client zip library in Arduino IDE.

# TODO

 - Finish TCP implementation with Arduino libraries in Context.cpp (and maybe internal_client.cpp)
 - Resolve using threads in send_status() function
 - Finish README