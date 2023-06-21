import zipfile as zf

with zf.ZipFile("internal-client-arduino.zip", "w") as f:
    f.write("include/internal_client.h", "internal-client-arduino/internal_client.h")
    f.write("include/device_management.h", "internal-client-arduino/device_management.h")
    f.write("include/general_error_codes.h", "internal-client-arduino/general_error_codes..h")
    f.write("include/ic_error_codes.h", "internal-client-arduino/ic_error_codes.h")
    f.write("include/memory_management.h", "internal-client-arduino/memory_management.h")
    f.write("source/internal_client.cpp", "internal-client-arduino/internal_client.cpp")
    f.write("source/protobuf/compiled/InternalProtocol.pb.h", "internal-client-arduino/InternalProtocol.pb.h")
    f.write("source/protobuf/compiled/InternalProtocol.pb.c", "internal-client-arduino/InternalProtocol.pb.c")    
