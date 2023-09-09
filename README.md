# TestProtobuf
protobuf的基础使用。工具环境：protobuf 4.0.x branch的源码，Visual Studio 2019，cmake-3.27.4-windows-x86_64

/*
* protobuf 开源地址：https://github.com/protocolbuffers/protobuf
* 本程序采用4.0.x分支的源码，因为main里的最新代码有更多的third_party依赖...
* 测试代码参考：https://protobuf.dev/getting-started/cpptutorial/
* 
* 基本步骤：
* 0. 完整下载protobuf源码（包括third_party目录里的submodule），编译出protoc.exe、libprotobuf.lib（Release版库）、libprotobufd.lib（Debug版库）
*    需要先使用CMake生成Visual Studio 2019工程（x64），然后再编译，参考文章：https://blog.csdn.net/m0_56410289/article/details/131167123
* 
* 1. 自定义数据格式.proto文件
* 2. 使用protoc编译器为.proto文件自动生成 序列化/反序列化的类
     protoc -I=$SRC_DIR --cpp_out=$DST_DIR $SRC_DIR/addressbook.proto
* 3. 在App工程里使用生成的.cc和.h文件
     VS工程配置：a) 把protobuf的源码src目录（e.g. D:\Dev\GitHub\protobuf\src）加到 C/C++ | 常规 | 附件包含目录
                 b) 在 链接器 | 输入 | 附加依赖项，Debug配置输入libprotobufd.lib、Release配置输入libprotobuf.lib
                 c) C/C++ | 代码生成 | 运行库，选择/MT
*/
