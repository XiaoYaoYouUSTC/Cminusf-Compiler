#include"logging.hpp"
// 引入头文件
int main(){

    LOG(DEBUG) << "This is DEBUG log item.";
    // 使用关键字LOG，括号中填入要输出的日志等级
    // 紧接着就是<<以及日志的具体信息，这一步就跟使用std::cout一样
    LOG(INFO) << "This is INFO log item";
    LOG(WARNING) << "This is WARNING log item";
    LOG(ERROR) << "This is ERROR log item";
    
    return 0;
}