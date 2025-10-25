@echo off
set PROTOC_PATH=D:\SoftWare\Developer\grpc\grpc-compile\third_party\protobuf\Debug\protoc.exe
set GRPC_PLUGIN_PATH=D:\SoftWare\Developer\grpc\grpc-compile\Debug\grpc_cpp_plugin.exe
set PROTO_FILE=message.proto

echo Generating gRPC code...
%PROTOC_PATH% -I="." --grpc_out="." --plugin=protoc-gen-grpc="%GRPC_PLUGIN_PATH%" "%PROTO_FILE%"

echo Generating gRPC code Done.

echo Generating C++ code...
%PROTOC_PATH% --cpp_out=. "%PROTO_FILE%"

echo Generating C++ code Done.
pause