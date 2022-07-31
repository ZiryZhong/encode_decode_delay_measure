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


int main() {

    DelayManager dm;

    dm.run("./config.yaml");

    return 0;
}