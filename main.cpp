/*
 * @Descripttion: 
 * @version: 
 * @Author: congsir
 * @Date: 2022-07-05 11:58:12
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2022-07-30 15:15:48
 */

#include<iostream>
#include "./utils/log.hpp"
#include "./utils/delay_manager.hpp"
#include <chrono>
// #include <thread>
#include <ctime>
#include <string.h>
#include <algorithm>
char * deleteSpace(char * str){
    char * tail = str;
    char * next = str;

    while(*next){ // 两个if可以合并到一块儿，这样写只是为了看着方便
        if(*next != ' '){ // 查找不是空格的字符
            *tail = *next;
            tail++;
        }
        if(*next == ' ' && *(next-1) != ' '){ // 只留一个空格的判断条件；当前字符为空格且前一个不为空格。
            *tail = *next;
            tail++;
        }
        next++;
    }
    *tail='\0'; // 字符串结束
    return str;
}

int main() {

    DelayManager dm;
    
    dm.run("./config.yaml");




    return 0;
}
