# chatsys-gPRC
环境自行搭建
创建protobuf文件，根据以下命令生成中间文件
```shell
$ protoc -I=. --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` msg.proto
$ protoc -I=. --cpp_out=. msg.proto
```
编译运行代码
```shell
$ mkdir build
$ cd build
$ cmake ..
$ make
```
