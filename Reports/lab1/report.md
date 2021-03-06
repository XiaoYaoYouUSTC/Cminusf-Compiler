# lab1实验报告

学号 PB18000051 姓名 陈俊驰

## 实验要求

​	根据cminux-f的词法补全文件lexical_analyer.l文件，完成词法分析器，能够输出识别出的token，type，line(刚出现的行数)，pos_start(该行开始位置)，pos_end(结束的位置,不包含)。具体的需识别的token见lexical_analyzer.h。其中部分token只需要被识别，但由于与程序运行无关，故不需要输出到分析结果。

## 实验难点

​	本实验的难点在于：1.如何书写正确的正则表达式来识别各类符号，尤其是识别注释、标识符、浮点数、整数、换行等；2.对于不同正则表达式优先级应合理排序，以保证token能被识别为应识别的结果；3.对于空格、换行、注释、错误等对程序运行无关的部分，应当特殊处理，考虑其对于各标识符位置的影响，但不输出。

## 实验设计

​	首先，根据了lexical_analyzer.h中的各项基本标识符类型，在flex补全部分表示出来，注意对于各类符号进行转义；然后，对于数值进行处理，根据cminus-f的对于整型和浮点数的正则表达式定义，在flex部分补全出来，注意，其长度非固定值；在之后对于注释，根据课后题所设计的正则表达式，表示出来，处于代码优雅性以及注释的复杂性，其动作应单独处理，包括考虑所包含的各种字符以及换行；对于换行，单独考虑，其动作为lines+=1，行尾变为1；对于空格，单独处理，不输出；对于错误，应输出其错误的范围。



## 实验结果验证

​	首先对于助教所提供的六个基本测试点进行测试，结果与助教所提供的结果一致，通过测试；

​	然后自己创造了一些测试样例：

​	a[]

​	.12

​	12.

​	1.1.1

​	/\*/\*/a

​	/\*\*ná**/a

​	/\*/\*\*/\*/a

​	结果为

​	a 285 1 1 2

​	[] 288 1 2 4

   .12 287 2 1 4

​	12. 287 3 1 4

​	1.1 287 4 1 4

​	.1 287 4  4 6

​	a 285 5 6 7

​	a 285 6 10 11

​	\* 261 7 7 8

​	/  262 7 8 9

​	a 285 7 9 10

​	其中，由于á占两个字节，故标识符a为该行第10个字节。上述运行结果与实际结果相同，程序正确。此外由于注释不能嵌套，多余的\*/会被识别为乘法和除法。

请提供部分自己的测试样例

## 实验反馈

​	在本次实验中，代码部分并不困难，难度主要对于所给实验要求的理解上以及对背景知识的掌握上，通过issue中的经验以及网上的教程，花费了不少时间掌握知识点；此外，对于字符的转义也是实验中的一个常出现的问题，如果对于\. ,\*等字符不进行转义，也会出现识别错误问题；由于对于C语言掌握的不好，忘记了switch语句会匹配以后一直执行到结束或者遇到break，也耽误了不少时间用来debug；对于注释的识别，由于对于合适结束有着错误的认识，导致了许多错误产生。本次实验中，系统的复习并时间了对于正则表达式相关的知识，了解了flex的使用及其功能，成功实现了助教的实验要求。

