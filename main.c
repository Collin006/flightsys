#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h> // 用于使用 _access 函数检查文件是否存在
#include "structure.h"
#include "flight_generator.h"

// 声明 generate_yearly_flight_info 函数
extern void generate_yearly_flight_info();

// 全局变量，用于存储航班信息链表的头指针
extern struct Flightlist* global_flight_head = NULL;

int main() {
    struct Flightlist* flightHead = NULL;

    // 打印欢迎提示信息
    printf("航班票务预订系统正在启动...\n");

    // 检查 flights.txt 文件是否存在
    if (_access("flights.txt", 0) != 0) {
        // 文件不存在，生成航班信息
        generate_yearly_flight_info();
    }

    // 加载航班信息
    load_flights(&flightHead);

    while (1) {
        printf("欢迎使用航班票务预订系统！\n");
        printf("1. 我是用户\n");
        printf("2. 我是管理员\n");
        printf("0. 退出\n");
        int choice;
        scanf("%d", &choice);
        switch (choice)
        {
        case 1: // 普通用户
            while (1) {
                printf("1. 注册\n");
                printf("2. 登录\n");
                printf("0. 返回\n");
                int choice1;
                scanf("%d", &choice1);
                switch (choice1)
                {
                case 1: // 注册
                    register_customer();
                    break;
                case 2: // 登录
                {
                    LoginResult loginResult = login();
                    if (loginResult.current != NULL) {
                        user_operation(loginResult.current, loginResult.head);
                        while (1) {
                            printf("1. 继续操作\n");
                            printf("0. 退出登录\n");
                            int sub_choice;
                            scanf("%d", &sub_choice);
                            if (sub_choice == 0) {
                                free_login_result(loginResult);
                                break;
                            }
                            else if (sub_choice == 1) {
                                user_operation(loginResult.current, loginResult.head);
                            }
                            else {
                                printf("无效选择，请重新输入。\n");
                            }
                        }
                    }
                    break;
                }
                case 0:
                    break;
                default:
                    break;
                }
                if (choice1 == 0)
                    break;
            }
            break;
        case 2: // 系统管理员
        {
            char input[7];
            int i = 0;
            char ch;
            printf("请输入单位管理员验证码: ");
            while (1) {
                ch = getch();  // 获取字符但不显示
                if (ch == '\r') {
                    input[i] = '\0';  // 字符串结束
                    printf("\n");
                    break;
                }
                // 如果是退格键，删除前一个字符
                else if (ch == '\b' && i > 0) {
                    i--;
                    printf("\b \b");
                }
                // 如果是可打印字符且未达到最大长度
                else if (i < 6 && (isprint(ch) || isspace(ch))) {
                    input[i++] = ch;
                    printf("*");
                }
            }
            if (strcmp(input, "FJHBCC") == 0) {
                // 调用 flight_operation 函数进行航班管理操作
                flight_operation();
            }
            else {
                printf("验证码输入错误\n");
            }
        }
        break;
        case 0:
            exit(0);
        default:
            break;
        }
    }
    return 0;
}