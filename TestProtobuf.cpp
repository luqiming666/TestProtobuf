// TestProtobuf.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "addressbook.pb.h"
#include "GameAssets.pb.h"

using namespace std;

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
                 d) 在xxx.pb.cc的属性页，C/C++ | 预编译头，选择“不使用预编译头”
*/

// This function fills in a Person message based on user input.
void PromptForAddress(tutorial::Person* person) {
    cout << "Enter person ID number: ";
    int id;
    cin >> id;
    person->set_id(id);
    cin.ignore(256, '\n');

    cout << "Enter name: ";
    getline(cin, *person->mutable_name());

    cout << "Enter email address (blank for none): ";
    string email;
    getline(cin, email);
    if (!email.empty()) {
        person->set_email(email);
    }

    while (true) {
        cout << "Enter a phone number (or leave blank to finish): ";
        string number;
        getline(cin, number);
        if (number.empty()) {
            break;
        }

        tutorial::Person::PhoneNumber* phone_number = person->add_phones();
        phone_number->set_number(number);

        cout << "Is this a mobile, home, or work phone? ";
        string type;
        getline(cin, type);
        if (type == "mobile") {
            phone_number->set_type(tutorial::Person::PHONE_TYPE_MOBILE);
        }
        else if (type == "home") {
            phone_number->set_type(tutorial::Person::PHONE_TYPE_HOME);
        }
        else if (type == "work") {
            phone_number->set_type(tutorial::Person::PHONE_TYPE_WORK);
        }
        else {
            cout << "Unknown phone type.  Using default." << endl;
        }
    }

    // hard-code for some new fields
    person->add_nickname("Jimmy");
    person->add_nickname("LuLu");
    person->add_nickname("Happydeer");

    char buffer[] = { 0x41, 0x42, 0x00, 0x43, 0x44, 0x45, 0x46 }; // test if we can read all data completely
    int length = sizeof(buffer) / sizeof(buffer[0]);
    person->set_secrets(buffer, length);
}

// Iterates though all people in the AddressBook and prints info about them.
void ListPeople(const tutorial::AddressBook& address_book) {
    for (int i = 0; i < address_book.people_size(); i++) {
        const tutorial::Person& person = address_book.people(i);

        cout << "Person ID: " << person.id() << endl;
        cout << "  Name: " << person.name() << endl;
        if (person.has_email()) {
            cout << "  E-mail address: " << person.email() << endl;
        }

        for (int j = 0; j < person.phones_size(); j++) {
            const tutorial::Person::PhoneNumber& phone_number = person.phones(j);

            switch (phone_number.type()) {
            case tutorial::Person::PHONE_TYPE_MOBILE:
                cout << "  Mobile phone #: ";
                break;
            case tutorial::Person::PHONE_TYPE_HOME:
                cout << "  Home phone #: ";
                break;
            case tutorial::Person::PHONE_TYPE_WORK:
                cout << "  Work phone #: ";
                break;
            }
            cout << phone_number.number() << endl;
        }

        // check the hard-coded new fields
        for (int k = 0; k < person.nickname_size(); k++) {
            cout << "Nickname: " << person.nickname(k) << endl;
        }
        const string& strScrets = person.secrets(); // 如果包含'\0'，则无法完整读取所有数据
        int bufsize = strScrets.size(); // 完整数据的长度，而非字符串长度
        int len = strScrets.length();
        char* pBuf = (char*) strScrets.c_str();

        char* pNewBuf = new char[bufsize];
        for (int aa = 0; aa < bufsize; aa++) {
            pNewBuf[aa] = pBuf[aa];
        }
        char* pNewBufOffset = pNewBuf + 3; // check the characters after '\0'
        delete[] pNewBuf;
    }
}

int CreateFirstPeople(string& pbFile)
{
    tutorial::AddressBook address_book;

    PromptForAddress(address_book.add_people());

    {
        // Write the new address book back to disk.
        fstream output(pbFile, ios::out | ios::trunc | ios::binary);
        if (!address_book.SerializeToOstream(&output)) {
            cerr << "Failed to write address book." << endl;
            return -1;
        }
    }

    return 0;
}

// Testing result > no compression at all!!!
//  bomb.he: 111607 bytes
//  assets.bnk: 111611 bytes
int TestAssetCompression()
{
    std::ifstream file("bomb.he");
    std::stringstream buffer;

    if (file.is_open()) { // 检查文件是否成功打开
        buffer << file.rdbuf(); 
        file.close();
    }
    else {
        std::cout << "无法打开文件" << std::endl;
        return 1;
    }

    std::string content = buffer.str();

    std::string assetFile = "assets.bnk";
    PBTester::AssetBank assetBank;
    //assetBank.set_he1(content.c_str(), content.length());
    assetBank.set_he1(content);
    //assetBank.set_he2(content);

    {
        fstream output(assetFile, ios::out | ios::trunc | ios::binary);
        assetBank.SerializeToOstream(&output);
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
bool gCreateFirstPeople = false; // Change it to true for the first run
bool gAddNewPeople = false; // To test adding a new people to the existing address_book

int main()
{
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    TestAssetCompression();

    string bookFile = "D:\\mybook.pb";

    if (gCreateFirstPeople) {
        CreateFirstPeople(bookFile);
    }

    tutorial::AddressBook address_book;

    {
        // Read the existing address book.
        fstream input(bookFile, ios::in | ios::binary);
        if (!input) {
            cout << bookFile << ": File not found.  Creating a new file." << endl;
        }
        else if (!address_book.ParseFromIstream(&input)) {
            cerr << "Failed to parse address book." << endl;
            return -1;
        }
    }

    if (gAddNewPeople) {

        PromptForAddress(address_book.add_people());

        {
            // Write the new address book back to disk.
            fstream output(bookFile, ios::out | ios::trunc | ios::binary);
            if (!address_book.SerializeToOstream(&output)) {
                cerr << "Failed to write address book." << endl;
                return -1;
            }
        }
    }

    cout << ">>> Let's see all people..." << endl;
    ListPeople(address_book);

    // Optional:  Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
