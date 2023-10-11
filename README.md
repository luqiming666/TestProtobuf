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

//  .proto文件语法：https://protobuf.dev/programming-guides/proto2/
//	1. 数据类型：bool, int32, int64，float, double, string；使用enum定义枚举类型。详见：https://protobuf.dev/programming-guides/proto2/#scalar
//  2. repeated表示数组，建议追加[packed = true]以提升编码效率
//  3. required vs. optional：required字段一旦定义之后不能修改，不能删除，不建议使用！（Proto3已经不再支持required）
//  4. 字段序号（Field Number）不能重复！一个字段一旦被分配了一个序号，不可修改。高频字段使用1~15的数字（这些字段编码时仅用一个字节，节省空间）
//  5. 如果要删除某个字段，其曾使用的field number必须保留（字段的名字也应该保留），即不可再用作其他字段；可以使用reserved来定义保留字段名字和序号
//  6. 一个字段一旦定义，类型和序号就不能再改变了！！！但可以与extension互转。
//	   例外：这些类型之间的转换是兼容的：（int32, uint32, int64, uint64, bool）、（sint32和sint6）、（string和bytes，若bytes是UTF-8）...
//  7. [under development] google.protobuf.Any表示任意类型，对其解析的.proto须在线部署
//  8. oneof 类似于C++的union，结构体里同一时间只有一个字段可用
//  9. map<key_type, value_type> map_field = N;  不能使用repeated, optional, or required
