#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>

// 用户信息结构体
struct cusman {
    char phonum[12];
    char code[13];
    char name[20];
    char idnumber[20];
    char sex[5];
    char note[50];
    int numberofflight;
    int points;
    struct Flightlist* purchased_flights;
};

// 航班结构体
struct Flight {
    char pdeparture[20];    // 出发地，需要对应机场名称
    char pdestination[20];  // 目的地，需要对应机场名称
    char tdaparture[30];    // 出发时间，需要对应标准的时间格式
    char tdestination[30];  // 到达时间，需要对应标准的时间格式
    char number[10];        // 航班编号
    int tnumber;            // 总座位数
    int seat_rows;          // 座位行数
    int seat_cols;          // 座位列数
    int first_class_num;    // 头等舱座位数
    int first_class_remain; // 头等舱剩余座位数
    float first_class_price;// 头等舱票价
    int business_class_num; // 商务舱座位数
    int business_class_remain; // 商务舱剩余座位数
    float business_class_price; // 商务舱票价
    int economy_class_num;  // 经济舱座位数
    int economy_class_remain; // 经济舱剩余座位数
    float economy_class_price; // 经济舱票价
};

// 航班链表结构体
struct Flightlist
{
    struct Flight data;
    char time[9];
    char seatnum[4];
    struct Flightlist* next;
    char passenger_name[20];   // 购票时填写的乘客姓名
    char passenger_id[20];     // 购票时填写的身份证号
};

// 用户链表结构体
struct cuslist
{
    struct cusman data;
    struct cuslist* next;
};

// 定义一个结构体用于存放登录结果
typedef struct {
    struct cuslist* current;
    struct cuslist* head;
} LoginResult;

// 从文件中加载用户信息
void loadcusman(struct cuslist** head);

// 注册用户
void register_customer();

// 用户登录
LoginResult login();

// 从文件中加载航班信息
void load_flights(struct Flightlist** head);

// 删除指定航班
void delete_flight(struct Flightlist** head, const char* flight_number);

// 显示剩余时间
void Display_remainingtime(long seconds);

// 加载所有用户信息
struct cuslist* load_all_customers();

// 计算剩余时间
long Calculate_time(char* departure_timestamp);

// 加载用户信息
void load_users(struct cuslist** head);

// 自定义不区分大小写的strstr函数
int strcasestr(const char* haystack, const char* needle);

// 用户选择座位
void chooseseat(struct Flightlist* current, int seatClass);

// 修改用户密码
void modify_password(struct cuslist* current, struct cuslist* head);

// 初始化航班信息
void initFlight(struct Flight* flight);

// 用户模糊查询自己的订票信息
void fuzzy_search_bookings(struct cusman* current_user);

// 获取用户输入的密码
void get_password(char* password, int max_length);

// 修改用户信息
void reviseinformation(struct cuslist* current_user, struct cuslist* head);

// 添加航班
void add_flight(struct Flightlist** head);

// 修改航班信息
void modify_flight(struct Flightlist* head, const char* flight_number);

// 查询航班信息
void query_flight(struct Flightlist* head, const char* flight_number);

// 释放航班链表内存
void free_flight_list(struct Flightlist* head);

// 航班管理操作
void flight_operation();

// 用户操作
void user_operation(struct cuslist* current, struct cuslist* head);

// 用户购票
void buy(struct Flightlist* head, struct cusman* customer);

// 查询用户已购买的航班
void query_purchased_flights(struct cusman* customer);

// 将所有航班信息保存到文件
void save_flights(struct Flightlist* head);

// 记录航班信息
void flightRecord(struct cusman* customer);

// 释放用户链表内存
void free_cuslist(struct cuslist* head);

// 清空输入缓冲区
void clear_input_buffer();

// 读取购票信息并修改
void read_purchase_info(struct Flightlist* newPurchased, struct cusman* customer, int seatClass);

// 加载用户已购买的航班信息
void load_purchased_flights(struct cusman* customer);

// 检查链表是否存在循环
int has_cycle(struct Flightlist* head);

// 释放登录结果结构体占用的内存
void free_login_result(LoginResult result);