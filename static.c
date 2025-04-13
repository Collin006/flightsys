#include "structure.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_DATE_DIFF 7  // 最大日期范围，可根据实际情况调整

// 声明全局变量，指向航班列表的头指针
extern struct Flightlist* global_flight_head;

// 自定义 strptime 函数
// 将日期字符串解析为 struct tm 结构体
char* strptime(const char* s, const char* f, struct tm* tm) {
    int year, month, day;
    if (sscanf(s, "%d-%d-%d", &year, &month, &day) == 3) {
        tm->tm_year = year - 1900;
        tm->tm_mon = month - 1;
        tm->tm_mday = day;
        tm->tm_hour = 0;
        tm->tm_min = 0;
        tm->tm_sec = 0;
        return (char*)(s + strlen(s));
    }
    return NULL;
}

// 将日期字符串转换为 time_t 类型
time_t str_to_time(const char* date_str) {
    struct tm tm = { 0 };
    if (strptime(date_str, "%Y-%m-%d", &tm) == NULL) {
        fprintf(stderr, "日期解析失败: %s\n", date_str);
        return -1;
    }
    return mktime(&tm);
}

// 从文件中加载客户信息
void loadcusman(struct cuslist** head) {
    FILE* file = fopen("customers.txt", "r");
    if (file == NULL) {
        // 如果原文件不存在，尝试从备份文件中加载
        file = fopen("customers_backup.txt", "r");
        if (file == NULL) {
            return;
        }
        printf("原用户数据文件不存在，从备份文件中加载数据...\n");
    }

    struct cuslist* current = NULL;
    struct cuslist* new_customer;
    char line[1024];  // 用于读取一行数据

    // 逐行读取文件
    while (fgets(line, sizeof(line), file) != NULL) {
        new_customer = (struct cuslist*)malloc(sizeof(struct cuslist));
        if (new_customer == NULL) {
            printf("内存分配失败！\n");
            fclose(file);
            return;
        }

        // 使用 sscanf 解析当前行
        int fields_read = sscanf(line, "%11s %13s %19s %19s %9s %4s %d",
            new_customer->data.phonum, new_customer->data.code, new_customer->data.name,
            new_customer->data.idnumber, new_customer->data.note, new_customer->data.sex,
            &new_customer->data.points);

        if (fields_read != 7) {
            // 格式不匹配，释放当前节点
            free(new_customer);
            continue;
        }

        new_customer->data.purchased_flights = NULL; // 初始化已购买航班列表
        new_customer->next = NULL;
        if (*head == NULL) {
            *head = new_customer;
            current = new_customer;
        }
        else {
            current->next = new_customer;
            current = new_customer;
        }
    }

    fclose(file);
}

// 将客户信息保存到文件，同时保存备份
void save_customers(struct cuslist* head) {
    // 打开原文件进行写入
    FILE* file = fopen("customers.txt", "w");
    if (file == NULL) {
        perror("无法打开客户文件进行写入");
        return;
    }

    struct cuslist* current = head;
    while (current != NULL) {
        fprintf(file, "%s %s %s %s %s %s %d %d\n",
            current->data.phonum,
            current->data.code,
            current->data.name,
            current->data.idnumber,
            current->data.sex,
            current->data.note,
            current->data.numberofflight,
            current->data.points);

        // 保存已购买航班信息
        struct Flightlist* flight = current->data.purchased_flights;
        while (flight != NULL) {
            fprintf(file, "  %s %s %s %s %s %s %s\n",
                flight->data.pdeparture,
                flight->data.pdestination,
                flight->data.tdaparture,
                flight->data.tdestination,
                flight->data.number,
                flight->seatnum,
                flight->passenger_name);
            flight = flight->next;
        }
        current = current->next;
    }

    fclose(file);

    // 打开备份文件进行写入
    FILE* backup_file = fopen("customers_backup.txt", "w");
    if (backup_file == NULL) {
        perror("无法打开备份文件进行写入");
        return;
    }

    current = head;
    while (current != NULL) {
        fprintf(backup_file, "%s %s %s %s %s %s %d %d\n",
            current->data.phonum,
            current->data.code,
            current->data.name,
            current->data.idnumber,
            current->data.sex,
            current->data.note,
            current->data.numberofflight,
            current->data.points);

        // 保存已购买航班信息
        struct Flightlist* flight = current->data.purchased_flights;
        while (flight != NULL) {
            fprintf(backup_file, "  %s %s %s %s %s %s %s\n",
                flight->data.pdeparture,
                flight->data.pdestination,
                flight->data.tdaparture,
                flight->data.tdestination,
                flight->data.number,
                flight->seatnum,
                flight->passenger_name);
            flight = flight->next;
        }
        current = current->next;
    }

    fclose(backup_file);
}

// 加载航班信息
void load_flights() {
    if (global_flight_head != NULL) {
        return; // 如果航班信息已加载，直接返回
    }

    FILE* file = fopen("flights.txt", "r");
    if (file == NULL) {
        perror("无法打开航班文件进行读取");
        return;
    }

    struct Flightlist* current = NULL;
    struct Flightlist* new_flight;
    char line[1024];
    int line_num = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        line_num++;
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        new_flight = (struct Flightlist*)malloc(sizeof(struct Flightlist));
        if (new_flight == NULL) {
            printf("内存分配失败！\n");
            fclose(file);
            return;
        }

        // 使用逗号分隔解析数据
        int fields_read = sscanf(line, "%19[^,],%19[^,],%29[^,],%29[^,],%9[^,],%d,%d,%d,%d,%d,%f,%d,%d,%f,%d,%d,%f",
            new_flight->data.pdeparture, new_flight->data.pdestination,
            new_flight->data.tdaparture, new_flight->data.tdestination,
            new_flight->data.number, &new_flight->data.tnumber,
            &new_flight->data.seat_rows, &new_flight->data.seat_cols,
            &new_flight->data.first_class_num, &new_flight->data.first_class_remain, &new_flight->data.first_class_price,
            &new_flight->data.business_class_num, &new_flight->data.business_class_remain, &new_flight->data.business_class_price,
            &new_flight->data.economy_class_num, &new_flight->data.economy_class_remain, &new_flight->data.economy_class_price);

        if (fields_read != 17) {
            free(new_flight);
            continue;
        }

        new_flight->next = NULL;
        if (global_flight_head == NULL) {
            global_flight_head = new_flight;
            current = new_flight;
        }
        else {
            current->next = new_flight;
            current = new_flight;
        }
    }

    fclose(file);
    // 打印加载的航班信息数量，可根据需要添加具体实现
}

// 获取用户输入的密码，隐藏输入内容
void get_password(char* password, int max_length) {
    int i = 0;
    char ch;
    while (1) {
        ch = getch();  // 获取用户输入但不显示
        if (ch == '\r') {
            password[i] = '\0';  // 字符串结束符
            printf("\n");
            break;
        }
        // 如果是退格键且有输入，删除前一个字符
        else if (ch == '\b' && i > 0) {
            i--;
            printf("\b \b");
        }
        // 如果是可打印字符且未超过最大长度，添加到密码中
        else if (i < max_length - 1 && (isprint(ch) || isspace(ch))) {
            password[i++] = ch;
            printf("*");
        }
    }
}

// 用户注册功能
void register_customer() {
    struct cusman new_customer;
    struct cuslist* new_node = (struct cuslist*)malloc(sizeof(struct cuslist));
    if (new_node == NULL) {
        perror("内存分配失败");
        return;
    }
    // 获取用户输入的手机号码
    while (1) {
        printf("请输入手机号码: ");
        scanf("%11s", new_customer.phonum);
        while (getchar() != '\n'); // 清空缓冲区
        if (strlen(new_customer.phonum) != 11) {
            printf("手机号码长度应为11位，请重新输入！\n");
            continue;
        }
        break;
    }
    // 获取用户输入的密码
    while (1) {
        printf("请输入密码(密码应为6位且不超过12位): ");
        get_password(new_customer.code, 13);
        if (strlen(new_customer.code) < 6 || strlen(new_customer.code) > 12) {
            printf("密码长度应在6到12位之间，请重新输入！\n");
            continue;
        }
        break;
    }
    // 获取用户输入的姓名
    printf("请输入姓名: ");
    scanf("%19s", new_customer.name);
    while (getchar() != '\n'); // 清空缓冲区
    // 获取用户输入的身份证号
    while (1) {
        printf("请输入身份证号: ");
        scanf("%19s", new_customer.idnumber);
        while (getchar() != '\n'); // 清空缓冲区
        if (strlen(new_customer.idnumber) != 18) {
            printf("身份证号长度应为18位，请重新输入！\n");
            continue;
        }
        break;
    }
    // 获取用户输入的性别
    printf("请输入性别: ");
    scanf("%4s", new_customer.sex);
    while (getchar() != '\n'); // 清空缓冲区
    // 询问用户是否需要填写备注
    printf("是否需要填写备注y or n\n");
    char keyword;
    scanf(" %c", &keyword);
    while (getchar() != '\n'); // 清空缓冲区
    if (keyword == 'y' || keyword == 'Y') {
        printf("请填写备注:");
        scanf("%49s", new_customer.note);
        while (getchar() != '\n'); // 清空缓冲区
    }
    else if (keyword == 'n' || keyword == 'N') {
        strcpy(new_customer.note, "无");
    }
    new_customer.purchased_flights = NULL;
    new_customer.numberofflight = 0;
    new_customer.points = 0;
    // 将新客户信息赋值给新节点
    new_node->data = new_customer;
    new_node->next = NULL;

    // 以追加模式打开文件
    FILE* file = fopen("customers.txt", "a");
    if (file == NULL) {
        perror("无法打开文件");
        free(new_node);
        return;
    }
    // 将新客户信息写入文件
    fprintf(file, "%s %s %s %s %s %s %d %d\n",
        new_node->data.phonum,
        new_node->data.code,
        new_node->data.name,
        new_node->data.idnumber,
        new_node->data.sex,
        new_node->data.note,
        new_node->data.numberofflight,
        new_node->data.points);
    // 关闭文件
    fclose(file);

    // 将新节点添加到客户列表头部
    struct cuslist* head = load_all_customers();
    new_node->next = head;
    head = new_node;

    // 保存更新后的用户信息
    save_customers(head);
}

// 从文件中加载用户信息
void load_users(struct cuslist** head) {
    FILE* file = fopen("customers.txt", "r");
    if (file == NULL) {
        printf("无法打开文件\n");
        return;
    }
    struct cuslist* current = NULL;
    char line[1024];
    // 逐行读取文件
    while (fgets(line, sizeof(line), file)) {
        struct cuslist* new_customer = (struct cuslist*)malloc(sizeof(struct cuslist));
        if (new_customer == NULL) {
            printf("内存分配失败\n");
            fclose(file);
            return;
        }
        // 分割每行的数据
        char* phonum = strtok(line, " ");
        char* code = strtok(NULL, " ");
        char* name = strtok(NULL, " ");
        char* idnumber = strtok(NULL, " ");
        char* sex = strtok(NULL, " ");
        char* note = strtok(NULL, " ");
        char* numberofflight = strtok(NULL, " ");
        char* points = strtok(NULL, "\n");

        // 将分割后的数据复制到结构体中
        strcpy(new_customer->data.phonum, phonum);
        strcpy(new_customer->data.code, code);
        strcpy(new_customer->data.name, name);
        strcpy(new_customer->data.idnumber, idnumber);
        strcpy(new_customer->data.sex, sex);
        strcpy(new_customer->data.note, note);
        new_customer->data.numberofflight = atoi(numberofflight);
        if (points != NULL) {
            new_customer->data.points = atoi(points);
        }
        else {
            new_customer->data.points = 0;
        }
        new_customer->data.purchased_flights = NULL;
        new_customer->next = NULL;
        if (*head == NULL) {
            *head = new_customer;
            current = *head;
        }
        else {
            current->next = new_customer;
            current = new_customer;
        }

        // 读取已购买的航班信息
        while (fgets(line, sizeof(line), file) && line[0] == ' ') {
            struct Flightlist* new_flight = (struct Flightlist*)malloc(sizeof(struct Flightlist));
            if (new_flight == NULL) {
                printf("内存分配失败\n");
                fclose(file);
                return;
            }
            char* pdeparture = strtok(line + 2, " ");
            char* pdestination = strtok(NULL, " ");
            char* tdaparture = strtok(NULL, " ");
            char* tdestination = strtok(NULL, " ");
            char* number = strtok(NULL, " ");
            char* seatnum = strtok(NULL, " ");
            char* passenger_name = strtok(NULL, "\n");

            strcpy(new_flight->data.pdeparture, pdeparture);
            strcpy(new_flight->data.pdestination, pdestination);
            strcpy(new_flight->data.tdaparture, tdaparture);
            strcpy(new_flight->data.tdestination, tdestination);
            strcpy(new_flight->data.number, number);
            strcpy(new_flight->seatnum, seatnum);
            strcpy(new_flight->passenger_name, passenger_name);

            new_flight->next = new_customer->data.purchased_flights;
            new_customer->data.purchased_flights = new_flight;
        }
    }
    fclose(file);
}

// 修改 login 函数
LoginResult login() {
    LoginResult result = { NULL, NULL };
    struct cuslist* head = NULL;
    load_users(&head);

    char phone[12];
    char code[13];

    // 循环获取用户输入的手机号码，确保长度为11位
    while (1) {
        printf("请输入手机号码: ");
        scanf("%11s", phone);
        if (strlen(phone) != 11) {
            printf("手机号码长度应为11位，请重新输入！\n");
            continue;
        }
        break;
    }

    printf("请输入密码: ");
    get_password(code, 13);

    struct cuslist* current = head;
    // 遍历用户列表，查找匹配的手机号码和密码
    while (current != NULL) {
        if (strcmp(current->data.phonum, phone) == 0 && strcmp(current->data.code, code) == 0) {
            result.current = current;
            result.head = head;

            // 加载用户已购买的航班信息
            load_purchased_flights(&current->data);

            // 输出登录成功欢迎信息
            printf("登录成功，欢迎%s\n", current->data.name);

            return result;
        }
        current = current->next;
    }

    printf("手机号码或密码错误\n");
    free_login_result(result);
    return result;
}

// 释放用户列表内存
void free_login_result(LoginResult result) {
    struct cuslist* temp;
    while (result.head != NULL) {
        temp = result.head;
        result.head = result.head->next;
        free(temp);
    }
}

// 将航班信息保存到文件
void save_flights(struct Flightlist* head) {
    // 打开原文件进行写入
    FILE* file = fopen("flights.txt", "w");
    if (file == NULL) {
        perror("无法打开航班文件进行写入");
        return;
    }

    struct Flightlist* current = head;
    while (current != NULL) {
        fprintf(file, "%s,%s,%s,%s,%s,%d,%d,%d,%d,%d,%.2f,%d,%d,%.2f,%d,%d,%.2f\n",
            current->data.pdeparture, current->data.pdestination,
            current->data.tdaparture, current->data.tdestination,
            current->data.number, current->data.tnumber,
            current->data.seat_rows, current->data.seat_cols,
            current->data.first_class_num, current->data.first_class_remain, current->data.first_class_price,
            current->data.business_class_num, current->data.business_class_remain, current->data.business_class_price,
            current->data.economy_class_num, current->data.economy_class_remain, current->data.economy_class_price);
        current = current->next;
    }

    fclose(file);

    // 打开备份文件进行写入
    FILE* backup_file = fopen("flights_backup.txt", "a");
    if (backup_file == NULL) {
        perror("无法打开备份文件进行写入");
        return;
    }

    current = head;
    while (current != NULL) {
        fprintf(backup_file, "%s,%s,%s,%s,%s,%d,%d,%d,%d,%d,%.2f,%d,%d,%.2f,%d,%d,%.2f\n",
            current->data.pdeparture, current->data.pdestination,
            current->data.tdaparture, current->data.tdestination,
            current->data.number, current->data.tnumber,
            current->data.seat_rows, current->data.seat_cols,
            current->data.first_class_num, current->data.first_class_remain, current->data.first_class_price,
            current->data.business_class_num, current->data.business_class_remain, current->data.business_class_price,
            current->data.economy_class_num, current->data.economy_class_remain, current->data.economy_class_price);
        current = current->next;
    }

    fclose(backup_file);
}

// 添加航班
void add_flight(struct Flightlist** head) {
    struct Flightlist* new_flight = (struct Flightlist*)malloc(sizeof(struct Flightlist));
    if (new_flight == NULL) {
        printf("内存分配失败！\n");
        return;
    }

    printf("请输入出发地: ");
    scanf("%s", new_flight->data.pdeparture);
    while (getchar() != '\n');  // 清除输入缓冲区的换行符

    printf("请输入目的地: ");
    scanf("%s", new_flight->data.pdestination);
    while (getchar() != '\n');  // 清除输入缓冲区的换行符

    printf("请输入出发时间（格式如：YYYY - MM - DD HH:MM:SS）: ");
    fgets(new_flight->data.tdaparture, sizeof(new_flight->data.tdaparture), stdin);
    // 去除 fgets 读取的换行符
    size_t len = strlen(new_flight->data.tdaparture);
    if (len > 0 && new_flight->data.tdaparture[len - 1] == '\n') {
        new_flight->data.tdaparture[len - 1] = '\0';
    }

    printf("请输入到达时间（格式如：YYYY - MM - DD HH:MM:SS）: ");
    fgets(new_flight->data.tdestination, sizeof(new_flight->data.tdestination), stdin);
    // 去除 fgets 读取的换行符
    len = strlen(new_flight->data.tdestination);
    if (len > 0 && new_flight->data.tdestination[len - 1] == '\n') {
        new_flight->data.tdestination[len - 1] = '\0';
    }

    printf("请输入航班编号: ");
    scanf("%s", new_flight->data.number);
    while (getchar() != '\n');  // 清除输入缓冲区的换行符

    printf("请输入总座位数: ");
    scanf("%d", &new_flight->data.tnumber);
    while (getchar() != '\n');  // 清除输入缓冲区的换行符

    printf("请输入座位行数: ");
    scanf("%d", &new_flight->data.seat_rows);
    while (getchar() != '\n');  // 清除输入缓冲区的换行符

    printf("请输入座位列数: ");
    scanf("%d", &new_flight->data.seat_cols);
    while (getchar() != '\n');  // 清除输入缓冲区的换行符

    printf("请输入头等舱座位数: ");
    scanf("%d", &new_flight->data.first_class_num);
    new_flight->data.first_class_remain = new_flight->data.first_class_num;
    while (getchar() != '\n');  // 清除输入缓冲区的换行符

    printf("请输入头等舱票价: ");
    scanf("%f", &new_flight->data.first_class_price);
    while (getchar() != '\n');  // 清除输入缓冲区的换行符

    printf("请输入商务舱座位数: ");
    scanf("%d", &new_flight->data.business_class_num);
    new_flight->data.business_class_remain = new_flight->data.business_class_num;
    while (getchar() != '\n');  // 清除输入缓冲区的换行符

    printf("请输入商务舱票价: ");
    scanf("%f", &new_flight->data.business_class_price);
    while (getchar() != '\n');  // 清除输入缓冲区的换行符

    printf("请输入经济舱座位数: ");
    scanf("%d", &new_flight->data.economy_class_num);
    new_flight->data.economy_class_remain = new_flight->data.economy_class_num;
    while (getchar() != '\n');  // 清除输入缓冲区的换行符

    printf("请输入经济舱票价: ");
    scanf("%f", &new_flight->data.economy_class_price);
    while (getchar() != '\n');  // 清除输入缓冲区的换行符

    new_flight->next = *head;
    *head = new_flight;

    printf("航班添加成功！\n");
    save_flights(*head); // 保存航班信息到文件
}

// 删除航班
void delete_flight(struct Flightlist** head, const char* flight_number) {
    char departure[20], destination[20], departure_date[11];
    // 提示用户输入要删除航班的出发地
    printf("请输入要删除航班的出发地: ");
    scanf("%19s", departure);
    // 提示用户输入要删除航班的目的地
    printf("请输入要删除航班的目的地: ");
    scanf("%19s", destination);
    // 提示用户输入要删除航班的出发日期
    printf("请输入要删除航班的出发日期（格式：YYYY-MM-DD）: ");
    scanf("%10s", departure_date);

    struct Flightlist* current = *head;
    struct Flightlist* previous = NULL;

    // 遍历航班列表，查找匹配的航班
    while (current != NULL &&
        (strcmp(current->data.number, flight_number) != 0 ||
            strcmp(current->data.pdeparture, departure) != 0 ||
            strcmp(current->data.pdestination, destination) != 0 ||
            strncmp(current->data.tdaparture, departure_date, 10) != 0)) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        // 未找到匹配航班，提示用户
        printf("未找到该航班。\n");
        return;
    }

    if (previous == NULL) {
        // 如果要删除的是头节点，更新头指针
        *head = current->next;
    }
    else {
        // 否则，将前一个节点的next指针指向当前节点的下一个节点
        previous->next = current->next;
    }

    // 释放当前节点的内存
    free(current);
    // 提示用户航班删除成功
    printf("航班删除成功！\n");
    // 保存更新后的航班信息到文件
    save_flights(*head);
}

// 修改航班信息
void modify_flight(struct Flightlist* head, const char* flight_number) {
    char departure[20], destination[20], departure_date[11];
    // 提示用户输入要修改航班的出发地
    printf("请输入要修改航班的出发地: ");
    scanf("%19s", departure);
    // 提示用户输入要修改航班的目的地
    printf("请输入要修改航班的目的地: ");
    scanf("%19s", destination);
    // 提示用户输入要修改航班的出发年月日
    printf("请输入要修改航班的出发年月日（格式：YYYY-MM-DD）: ");
    scanf("%10s", departure_date);

    struct Flightlist* current = head;
    // 遍历航班列表，查找匹配的航班
    while (current != NULL &&
        (strcmp(current->data.number, flight_number) != 0 ||
            strcmp(current->data.pdeparture, departure) != 0 ||
            strcmp(current->data.pdestination, destination) != 0 ||
            strncmp(current->data.tdaparture, departure_date, 10) != 0)) {
        current = current->next;
    }

    if (current == NULL) {
        // 未找到匹配航班，提示用户
        printf("未找到该航班！\n");
        return;
    }

    // 提示用户输入新的出发地
    printf("请输入新的出发地: ");
    scanf("%s", current->data.pdeparture);
    // 提示用户输入新的目的地
    printf("请输入新的目的地: ");
    scanf("%s", current->data.pdestination);

    // 提示用户输入新的出发时间
    printf("请输入新的出发时间（格式如：YYYY-MM-DD HH:MM:SS）: ");
    scanf("%s", current->data.tdaparture);
    // 清除缓冲区中的换行符
    while (getchar() != '\n');

    // 提示用户输入新的落地时间
    printf("请输入新的落地时间（格式如：YYYY-MM-DD HH:MM:SS）: ");
    scanf("%s", current->data.tdestination);
    // 清除缓冲区中的换行符
    while (getchar() != '\n');

    // 提示用户输入新的总座位数
    printf("请输入新的总座位数: ");
    scanf("%d", &current->data.tnumber);
    // 提示用户输入新的座位行数
    printf("请输入新的座位行数: ");
    scanf("%d", &current->data.seat_rows);
    // 提示用户输入新的座位列数
    printf("请输入新的座位列数: ");
    scanf("%d", &current->data.seat_cols);
    // 提示用户输入新的头等舱座位数
    printf("请输入新的头等舱座位数: ");
    scanf("%d", &current->data.first_class_num);
    current->data.first_class_remain = current->data.first_class_num;
    // 提示用户输入新的头等舱价格
    printf("请输入新的头等舱价格: ");
    scanf("%f", &current->data.first_class_price);
    // 提示用户输入新的商务舱座位数
    printf("请输入新的商务舱座位数: ");
    scanf("%d", &current->data.business_class_num);
    current->data.business_class_remain = current->data.business_class_num;
    // 提示用户输入新的商务舱价格
    printf("请输入新的商务舱价格: ");
    scanf("%f", &current->data.business_class_price);
    // 提示用户输入新的经济舱座位数
    printf("请输入新的经济舱座位数: ");
    scanf("%d", &current->data.economy_class_num);
    current->data.economy_class_remain = current->data.economy_class_num;
    // 提示用户输入新的经济舱价格
    printf("请输入新的经济舱价格: ");
    scanf("%f", &current->data.economy_class_price);

    // 提示用户航班信息修改成功
    printf("航班信息修改成功！\n");
    // 保存航班信息到文件
    save_flights(head);
}

// 查询航班信息
void query_flight(struct Flightlist* head) {
    char departure[20], destination[20];
    char start_date[11], end_date[11];
    int max_days = 7; // 限制日期范围的最大天数

    // 提示用户输入出发地
    printf("请输入出发地: ");
    scanf("%19s", departure);
    // 提示用户输入目的地
    printf("请输入目的地: ");
    scanf("%19s", destination);
    // 提示用户输入出发日期范围
    printf("请输入出发日期范围（格式: YYYY-MM-DD YYYY-MM-DD，最大范围 %d 天  示例：2025-01-01 2025-01-07）: ", max_days);
    scanf("%10s %10s", start_date, end_date);

    // 检查日期范围是否超过最大天数
    time_t time_start = str_to_time(start_date);
    time_t time_end = str_to_time(end_date);
    if (time_start == -1 || time_end == -1) {
        // 日期解析失败，提示用户输入正确的日期格式
        printf("日期解析失败，请输入正确的日期格式。\n");
        return;
    }
    double diff = difftime(time_end, time_start) / (60 * 60 * 24);
    if (diff > max_days) {
        // 日期范围超过最大天数，提示用户重新输入
        printf("日期范围超过 %d 天，请重新输入。\n", max_days);
        return;
    }

    struct Flightlist* current = head;
    int found = 0;
    // 遍历航班列表，查找符合条件的航班
    while (current != NULL) {
        // 提取出发日期
        char flight_date[11];
        strncpy(flight_date, current->data.tdaparture, 10);
        flight_date[10] = '\0';

        time_t time_flight = str_to_time(flight_date);
        if (time_flight == -1) {
            // 跳过日期解析失败的航班
            current = current->next;
            continue;
        }

        // 检查是否符合条件
        if (strcmp(current->data.pdeparture, departure) == 0 &&
            strcmp(current->data.pdestination, destination) == 0 &&
            time_flight >= time_start && time_flight <= time_end) {
            // 输出符合条件的航班信息
            printf("出发地: %s\n", current->data.pdeparture);
            printf("目的地: %s\n", current->data.pdestination);
            printf("出发时间: %s\n", current->data.tdaparture);
            printf("落地时间: %s\n", current->data.tdestination);
            printf("航班编号: %s\n", current->data.number);
            printf("总座位数: %d\n", current->data.tnumber);
            printf("座位行数: %d\n", current->data.seat_rows);
            printf("座位列数: %d\n", current->data.seat_cols);
            printf("头等舱座位数: %d, 剩余座位数: %d, 价格: %.2f\n", current->data.first_class_num, current->data.first_class_remain, current->data.first_class_price);
            printf("商务舱座位数: %d, 剩余座位数: %d, 价格: %.2f\n", current->data.business_class_num, current->data.business_class_remain, current->data.business_class_price);
            printf("经济舱座位数: %d, 剩余座位数: %d, 价格: %.2f\n", current->data.economy_class_num, current->data.economy_class_remain, current->data.economy_class_price);
            printf("------------------------\n");
            found = 1;
        }
        current = current->next;
    }
    if (!found) {
        // 未找到符合条件的航班，提示用户
        printf("未找到符合条件的航班！\n");
    }
}

// 释放链表内存
void free_flight_list(struct Flightlist* head) {
    struct Flightlist* temp;
    // 遍历链表，释放每个节点的内存
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

// 飞行操作函数
void flight_operation() {
    // 直接使用全局变量
    struct Flightlist* flightHead = global_flight_head;
    // 加载航班信息
    load_flights(&flightHead);

    while (1) {
        // 显示操作菜单
        printf("1. 添加航班\n");
        printf("2. 删除航班\n");
        printf("3. 修改航班信息\n");
        printf("4. 查询航班信息\n");
        printf("0. 退出\n");
        int choice;
        scanf("%d", &choice);
        char flightNumber[10];

        switch (choice) {
        case 1:
            // 调用添加航班函数
            add_flight(&flightHead);
            break;
        case 2:
            // 提示用户输入要删除的航班号
            printf("请输入要删除的航班号: ");
            scanf("%s", flightNumber);
            // 调用删除航班函数
            delete_flight(&flightHead, flightNumber);
            break;
        case 3:
            // 提示用户输入要修改的航班号
            printf("请输入要修改的航班号: ");
            scanf("%s", flightNumber);
            // 调用修改航班信息函数
            modify_flight(flightHead, flightNumber);
            break;
        case 4:
            // 调用高级查询函数
            query_flight(flightHead);
            break;
        case 0:
            // 释放链表内存
            free_flight_list(flightHead);
            return;
        default:
            // 无效的选择，提示用户重新输入
            printf("无效的选择，请重新输入！\n");
        }
    }
}

// 定义 load_all_customers 函数，用于加载所有用户信息
struct cuslist* load_all_customers() {
    struct cuslist* head = NULL;
    // 调用 load_users 函数加载用户信息
    load_users(&head);
    return head;
}

// 读取购票信息并保存到文件，同时更新用户已购航班列表
void read_purchase_info(struct Flightlist* newPurchased, struct cusman* customer, int seatClass) {
    char name[20];
    char id[20];

    printf("请输入乘客姓名: ");
    fgets(name, sizeof(name), stdin);
    size_t len = strlen(name);
    if (len > 0 && name[len - 1] == '\n') {
        name[len - 1] = '\0';
    }
    strcpy(newPurchased->passenger_name, name);

    printf("请输入乘客身份证号: ");
    fgets(id, sizeof(id), stdin);
    len = strlen(id);
    if (len > 0 && id[len - 1] == '\n') {
        id[len - 1] = '\0';
    }
    strcpy(newPurchased->passenger_id, id);

    // 打印出票提示信息
    printf("正在出票中，请耐心等候\n");

    FILE* fp = fopen("ticket_info.txt", "a");
    if (fp == NULL) {
        perror("无法打开文件");
        return;
    }

    const char* seat_class_name;
    switch (seatClass) {
    case 1:
        seat_class_name = "头等舱";
        break;
    case 2:
        seat_class_name = "商务舱";
        break;
    case 3:
        seat_class_name = "经济舱";
        break;
    default:
        seat_class_name = "未知舱位";
    }

    // 将购票信息写入文件
    fprintf(fp, "乘客姓名:%s\n", newPurchased->passenger_name);
    fprintf(fp, "身份证号:%s\n", newPurchased->passenger_id);
    fprintf(fp, "航班号:%s\n", newPurchased->data.number);
    fprintf(fp, "出发时间:%s\n", newPurchased->data.tdaparture);
    fprintf(fp, "舱位类型:%s\n", seat_class_name);
    fprintf(fp, "用户手机号:%s\n\n", customer->phonum);

    fclose(fp);

    // 更新用户的 purchased_flights 列表
    newPurchased->next = customer->purchased_flights;
    customer->purchased_flights = newPurchased;
    customer->numberofflight++;
    save_customers(load_all_customers()); // 保存所有用户信息
}

// 购买航班票的函数
void buy(struct Flightlist* head, struct cusman* customer) {
    char flightNumber[10];
    char departure[20];
    char destination[20];
    char departureDate[11];
    int seatClass;

    printf("请输入您要购买的航班编号:");
    scanf("%s", flightNumber);
    printf("请输入出发地:");
    scanf("%s", departure);
    printf("请输入目的地:");
    scanf("%s", destination);
    printf("请输入出发时间(格式：YYYY-MM-DD):");
    scanf("%10s", departureDate);

    struct Flightlist* current = head;
    // 查找匹配的航班
    while (current != NULL && (strcmp(current->data.number, flightNumber) != 0 ||
        strcmp(current->data.pdeparture, departure) != 0 ||
        strcmp(current->data.pdestination, destination) != 0 ||
        strncmp(current->data.tdaparture, departureDate, 10) != 0)) {
        current = current->next;
    }

    if (current == NULL) {
        printf("未找到该航班。\n");
        return;
    }

    printf("请选择座位类型：1. 头等舱 2. 商务舱 3. 经济舱: ");
    scanf("%d", &seatClass);
    while (getchar() != '\n');

    float price = 0;
    switch (seatClass) {
    case 1:
        if (current->data.first_class_remain > 0) {
            current->data.first_class_remain--;
            price = current->data.first_class_price;
        }
        else {
            printf("头等舱已无剩余座位。\n");
            return;
        }
        break;
    case 2:
        if (current->data.business_class_remain > 0) {
            current->data.business_class_remain--;
            price = current->data.business_class_price;
        }
        else {
            printf("商务舱已无剩余座位。\n");
            return;
        }
        break;
    case 3:
        if (current->data.economy_class_remain > 0) {
            current->data.economy_class_remain--;
            price = current->data.economy_class_price;
        }
        else {
            printf("经济舱已无剩余座位。\n");
            return;
        }
        break;
    default:
        printf("无效的座位类型选择。\n");
        return;
    }

    printf("购票%s成功，票价为%.2f\n", seatClass == 1 ? "头等舱" : (seatClass == 2 ? "商务舱" : "经济舱"), price);
    struct Flightlist* newPurchased = (struct Flightlist*)malloc(sizeof(struct Flightlist));
    newPurchased->data = current->data;
    read_purchase_info(newPurchased, customer, seatClass);

    // 确保新节点正确添加到链表中
    newPurchased->next = customer->purchased_flights;
    customer->purchased_flights = newPurchased;
    save_flights(head);
    printf("出票成功！\n");
}

// 查询用户已预订的航班信息
void query_purchased_flights(struct cusman* customer) {
    struct Flightlist* current = customer->purchased_flights;
    if (current == NULL) {
        printf("您没有预订任何航班。\n");
        return;
    }

    // 用于记录已经访问过的节点，避免循环访问
    struct Flightlist** visited = (struct Flightlist**)malloc(1000 * sizeof(struct Flightlist*));
    int visited_count = 0;

    while (current != NULL) {
        int is_visited = 0;
        for (int i = 0; i < visited_count; i++) {
            if (visited[i] == current) {
                is_visited = 1;
                break;
            }
        }
        if (is_visited) {
            break;
        }
        visited[visited_count++] = current;

        printf("出发地: %s\n", current->data.pdeparture);
        printf("目的地: %s\n", current->data.pdestination);
        printf("出发时间: %s\n", current->data.tdaparture);
        printf("到达时间: %s\n", current->data.tdestination);
        printf("航班编号: %s\n", current->data.number);
        printf("------------------------\n");
        current = current->next;
    }

    free(visited);
}

// 加载用户已购买的航班信息，避免重复加载
void load_purchased_flights(struct cusman* customer) {
    if (customer->purchased_flights != NULL) {
        return; // 如果已经加载过，直接返回
    }
    FILE* fp = fopen("ticket_info.txt", "r");
    if (fp == NULL) {
        return;
    }

    char line[100];
    char name[20], id[20], flight_number[10], departure_time[20], seat_class[20], phone[12];

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (sscanf(line, "乘客姓名:%19s", name) == 1) {
            fgets(line, sizeof(line), fp);
            sscanf(line, "身份证号:%19s", id);
            fgets(line, sizeof(line), fp);
            sscanf(line, "航班编号:%9s", flight_number);
            fgets(line, sizeof(line), fp);
            sscanf(line, "出发时间:%19s", departure_time);
            fgets(line, sizeof(line), fp);
            sscanf(line, "舱位类型:%19s", seat_class);
            fgets(line, sizeof(line), fp);
            sscanf(line, "用户手机号:%11s", phone);

            if (strcmp(phone, customer->phonum) == 0) {
                struct Flightlist* new_flight = (struct Flightlist*)malloc(sizeof(struct Flightlist));
                if (new_flight == NULL) {
                    printf("内存分配失败！\n");
                    continue;
                }

                // 根据航班编号查找航班详细信息
                struct Flightlist* flight_head = NULL;
                load_flights(&flight_head);
                struct Flightlist* current = flight_head;
                while (current != NULL) {
                    if (strcmp(current->data.number, flight_number) == 0) {
                        new_flight->data = current->data;
                        break;
                    }
                    current = current->next;
                }

                strcpy(new_flight->passenger_name, name);
                strcpy(new_flight->passenger_id, id);
                strcpy(new_flight->seatnum, seat_class);

                new_flight->next = customer->purchased_flights;
                customer->purchased_flights = new_flight;
                customer->numberofflight++;
            }
        }
    }

    fclose(fp);
}

// 修改用户密码
void modify_password(struct cuslist* current, struct cuslist* head) {
    char password2[15];
    int passwordmatch = 0;
    if (current == NULL) {
        printf("请先登录！\n");
        return;
    }

    char old_password[13];
    char new_password[13];

    printf("请输入旧密码: ");
    get_password(old_password, sizeof(old_password));

    if (strcmp(current->data.code, old_password) != 0) {
        printf("旧密码输入错误！\n");
        return;
    }

    while (passwordmatch == 0) {
        printf("请输入新密码: \n");
        get_password(new_password, sizeof(new_password));

        printf("请再次输入: \n");
        get_password(password2, sizeof(password2));

        if (strcmp(new_password, password2) == 0) {
            strcpy(current->data.code, new_password);
            passwordmatch = 1;
            printf("密码修改成功！\n");
        }
        else {
            printf("两次输入的密码不一致，请重新输入！\n");
        }
    }

    strcpy(current->data.code, new_password);

    printf("密码修改成功！\n");
}

// 清除输入缓冲区，避免残留字符影响后续输入
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// 显示用户的飞行记录
void flightRecord(struct cusman* customer) {
    // 简单假设飞行记录就是已购买的航班记录
    query_purchased_flights(customer);
}

// 自定义的不区分大小写的字符串查找函数，返回 1 表示匹配，0 表示不匹配
int strcasestr(const char* haystack, const char* needle) {
    if (!*needle) return 1; // 如果 needle 为空，则认为匹配
    for (; *haystack; haystack++) {
        if (tolower(*haystack) == tolower(*needle)) {
            const char* h = haystack;
            const char* n = needle;
            while (*h && *n && tolower(*h) == tolower(*n)) {
                h++;
                n++;
            }
            if (!*n) return 1;
        }
    }
    return 0;
}

// 计算距离航班出发的剩余时间
long Calculate_time(const char* departure_time) {
    struct tm tm = { 0 };
    // 解析时间字符串
    if (strptime(departure_time, "%Y-%m-%d %H:%M:%S", &tm) == NULL) {
        // 如果解析失败，尝试只解析日期部分
        if (strptime(departure_time, "%Y-%m-%d", &tm) == NULL) {
            return -1; // 解析失败，返回 -1
        }
    }
    time_t flight_time = mktime(&tm);
    time_t current_time = time(NULL);
    return flight_time - current_time;
}

// 显示距离航班出发的剩余时间，并给出安检提醒
void Display_remainingtime(long seconds) {
    int security_checktime = 2 * 3600;
    if (seconds < 0) {
        printf("该航班已起飞\n");
    }
    else {
        int days = seconds / (24 * 3600);
        seconds %= (24 * 3600);
        int hours = seconds / 3600;
        seconds %= 3600;
        int minutes = seconds / 60;
        seconds %= 60;

        if (days > 0) {
            printf("距离航班起飞还有%d天%d小时%d分钟%d秒\n", days, hours, minutes, seconds);
        }
        else if (hours > 0) {
            printf("距离航班起飞还有%d小时%d分钟%d秒\n", hours, minutes, seconds);
        }
        else if (minutes > 0) {
            printf("距离航班起飞还有%d分钟%d秒\n", minutes, seconds);
        }
        else {
            printf("距离航班起飞还有%d秒\n", seconds);
        }

        // 安检提醒
        if ((hours * 3600 + minutes * 60 + seconds) < security_checktime) {
            printf("温馨提示：距离航班起飞不足2小时，请尽快前往安检！\n");
        }
    }
}

// 用户模糊查询自己的航班预订信息
void fuzzy_search_bookings(struct cusman* current_user) {
    if (current_user == NULL) {
        printf("当前用户未登录。\n");
        return;
    }

    char input[200];
    // 清除输入缓冲区
    while (getchar() != '\n');
    printf("请输入关键词，关键词之间用空格分隔，最多输入十个关键词: ");
    fgets(input, sizeof(input), stdin);
    // 清理字符串末尾的换行符
    input[strcspn(input, "\n")] = 0;

    // 分割关键词
    char* keywords[10];
    int keyword_count = 0;

    char* token = strtok(input, " "); // 分割关键词到keywords数组中
    while (token != NULL && keyword_count < 10) {
        keywords[keyword_count++] = token;
        token = strtok(NULL, " ");
    }

    if (keyword_count == 0) {
        printf("请输入至少一个关键词。\n");
        return;
    }

    struct Flightlist* flight = current_user->purchased_flights;
    // 用于标记已经访问过的节点
    struct Flightlist** visited = (struct Flightlist**)malloc(1000 * sizeof(struct Flightlist*));
    int visited_count = 0;

    int found = 0;

    while (flight != NULL) {
        int is_visited = 0;
        for (int i = 0; i < visited_count; i++) {
            if (visited[i] == flight) {
                is_visited = 1;
                break;
            }
        }
        if (is_visited) {
            break;
        }
        visited[visited_count++] = flight;

        int all_keywords_match = 1; // 用于记录是否匹配所有关键词
        for (int i = 0; i < keyword_count; i++) {
            if (!strcasestr(flight->data.number, keywords[i]) && !strcasestr(flight->data.pdeparture, keywords[i]) && !strcasestr(flight->data.pdestination, keywords[i])) {
                all_keywords_match = 0;
                break;
            }
        }

        if (all_keywords_match) {
            printf("\n乘客姓名:%s\n", flight->passenger_name);
            printf("证件号码:%s\n", flight->passenger_id);
            printf("航班号:%s\n", flight->data.number);
            printf("航程:%s 至 %s\n", flight->data.pdeparture, flight->data.pdestination);
            printf("出发时间:%s\n", flight->data.tdaparture);
            long remaining_seconds = Calculate_time(&flight->data.tdaparture); // 确保 Calculate_time 正确实现
            Display_remainingtime(remaining_seconds); // 显示剩余时间
            found = 1;
        }
        flight = flight->next;
    }

    free(visited);

    if (!found) {
        printf("未找到匹配的航班记录。\n");
    }
}

// 修改订单信息
int reflightinformation(struct cuslist* current, struct cuslist* head) {
    if (current == NULL) {
        printf("错误: 用户信息为空!\n");
        return 0;
    }
    int count = 0;
    // 显示已预订的航班信息
    struct Flightlist* temp = current->data.purchased_flights;
    // 用于标记已经访问过的节点
    struct Flightlist** visited = (struct Flightlist**)malloc(1000 * sizeof(struct Flightlist*));
    int visited_count = 0;

    if (temp == NULL) {
        free(visited);
        printf("您还没有预订任何航班。\n");
        return 0;
    }
    // 统计已预订的航班数量
    while (temp != NULL) {
        int is_visited = 0;
        for (int i = 0; i < visited_count; i++) {
            if (visited[i] == temp) {
                is_visited = 1;
                break;
            }
        }
        if (is_visited) {
            break;
        }
        visited[visited_count++] = temp;

        printf("%d. 出发地: %s -> 目的地: %s, 出发时间: %s, 航班号: %s\n", count + 1, temp->data.pdeparture, temp->data.pdestination, temp->data.tdaparture, temp->data.number);
        count++;
        temp = temp->next;
    }

    int choice;
    printf("请选择要修改的航班序号(1-%d)\n", count);
    // 获取用户选择
    if (scanf("%d", &choice) != 1) {
        printf("输入无效，请重新输入: ");
        free(visited);
        return 0;
    }
    // 清除输入缓冲区的换行符
    getchar();
    if (choice < 1 || choice > count) {
        printf("无效选择\n");
        free(visited);
        return 0;
    }
    struct Flightlist* selected_flight = current->data.purchased_flights;
    // 找到用户选择的航班
    for (int i = 1; i < choice; i++) {
        selected_flight = selected_flight->next;
    }
    int modify_choice;
    while (1) {
        printf("请选择要修改的项目:\n");
        printf("1. 修改乘客姓名\n");
        printf("2. 修改证件号码\n");
        printf("3. 退票\n");
        printf("4. 返回\n");
        printf("请输入您的选择 (1-4): ");
        // 获取用户选择
        if (scanf("%d", &modify_choice) != 1) {
            printf("输入无效，请重新输入: ");
            // 清除输入缓冲区
            while (getchar() != '\n');
            continue;
        }
        // 清除输入缓冲区的换行符
        getchar();
        if (modify_choice == 4) {
            break;
        }
        switch (modify_choice) {
        case 1:
            printf("请输入新的乘客姓名 (当前: %s): ", selected_flight->passenger_name);
            fgets(selected_flight->passenger_name, sizeof(selected_flight->passenger_name), stdin);
            selected_flight->passenger_name[strcspn(selected_flight->passenger_name, "\n")] = '\0';
            printf("姓名已更新为: %s\n", selected_flight->passenger_name);
            break;
        case 2:
            printf("请输入新的证件号码 (当前: %s): ", selected_flight->passenger_id);
            fgets(selected_flight->passenger_id, sizeof(selected_flight->passenger_id), stdin);
            selected_flight->passenger_id[strcspn(selected_flight->passenger_id, "\n")] = '\0';
            printf("证件号码已更新为: %s\n", selected_flight->passenger_id);
            break;
            // 修改后的退票逻辑
        case 3:
            // 退票逻辑：修改座位状态为未占用，从预订单列表中移除该航班
            if (selected_flight->data.first_class_remain < selected_flight->data.first_class_num) {
                selected_flight->data.first_class_remain++;
            }
            else if (selected_flight->data.business_class_remain < selected_flight->data.business_class_num) {
                selected_flight->data.business_class_remain++;
            }
            else if (selected_flight->data.economy_class_remain < selected_flight->data.economy_class_num) {
                selected_flight->data.economy_class_remain++;
            }

            struct Flightlist* prev = NULL;
            struct Flightlist* current_flight = current->data.purchased_flights;
            while (current_flight != NULL && current_flight != selected_flight) {
                prev = current_flight;
                current_flight = current_flight->next;
            }
            if (current_flight == selected_flight) {
                if (prev == NULL) {
                    current->data.purchased_flights = current_flight->next;
                }
                else {
                    prev->next = current_flight->next;
                }

                // 删除 ticket_info.txt 中的对应信息
                FILE* fp = fopen("ticket_info.txt", "r");
                if (fp == NULL) {
                    perror("无法打开文件");
                }
                else {
                    FILE* temp_fp = fopen("temp.txt", "w");
                    if (temp_fp == NULL) {
                        perror("无法创建临时文件");
                        fclose(fp);
                    }
                    else {
                        char line[100];
                        int skip = 0;
                        while (fgets(line, sizeof(line), fp) != NULL) {
                            if (strstr(line, selected_flight->passenger_name) != NULL) {
                                skip = 1;
                                for (int i = 0; i < 5; i++) {
                                    fgets(line, sizeof(line), fp);
                                }
                            }
                            else if (skip) {
                                if (strstr(line, "乘客姓名:") != NULL) {
                                    skip = 0;
                                }
                            }
                            if (!skip) {
                                fputs(line, temp_fp);
                            }
                        }
                        fclose(fp);
                        fclose(temp_fp);
                        remove("ticket_info.txt");
                        rename("temp.txt", "ticket_info.txt");
                    }
                }

                free(current_flight);
                current_flight = NULL; // 避免悬空指针
                selected_flight = NULL; // 避免悬空指针
                printf("退票成功\n");
                free(visited);
                return 0; // 直接退出菜单
            }
            break;
        }
    }
    free(visited);
    return 1;
}

// 修改用户信息的函数
void reviseinformation(struct cuslist* current_user, struct cuslist* head) {
    if (current_user == NULL) {
        printf("当前用户信息为空\n");
        return;
    }

    while (1) {
        // 显示当前用户信息
        printf("\n=== 当前用户信息 ===\n");
        printf("1. 姓名: %s\n", current_user->data.name);
        printf("2. 手机号码: %s\n", current_user->data.phonum);
        printf("3. 身份证号: %s\n", current_user->data.idnumber);
        printf("4. 性别: %s\n", current_user->data.sex);
        printf("5. 密码:\n");
        printf("6. 备注信息: %s\n", current_user->data.note);
        printf("7. 退出修改系统\n");

        printf("\n请输入要修改的信息项编号(1-7): ");
        int choice;
        if (scanf("%d", &choice) != 1) {
            printf("输入无效，请输入一个整数\n");
            while (getchar() != '\n'); // 清空输入缓冲区
            continue;
        }

        if (choice == 7) {
            break;
        }

        if (choice < 1 || choice > 7) {
            printf("输入无效，请输入1-7之间的整数\n");
            continue;
        }

        switch (choice) {
        case 1:
            printf("请输入新的姓名: ");
            scanf("%s", current_user->data.name);
            break;

        case 2:
            while (1) {
                printf("请输入新的手机号码(11位): ");
                scanf("%11s", current_user->data.phonum);
                if (strlen(current_user->data.phonum) != 11) {
                    printf("手机号码应为11位，请重新输入\n");
                    continue;
                }
                break;
            }
            break;

        case 3:
            while (1) {
                printf("请输入新的身份证号: ");
                scanf("%s", current_user->data.idnumber);
                if (strlen(current_user->data.idnumber) != 18) {
                    printf("身份证号应为18位，请重新输入\n");
                    continue;
                }
                break;
            }
            break;

        case 4:
            printf("请输入新的性别: ");
            scanf("%s", current_user->data.sex);
            break;

        case 5:
            modify_password(current_user, head);
            break;

        case 6:
            printf("请输入新的备注信息: ");
            scanf("%s", current_user->data.note);
            break;
        }

        // 更新修改后的用户信息到文件
        FILE* file = fopen("customers.txt", "w");
        if (file == NULL) {
            printf("无法打开文件\n");
            return;
        }
        // 将所有用户的信息写入文件
        struct cuslist* temp = head;
        while (temp != NULL) {
            fprintf(file, "%s %s %s %s %s %s %d %d\n",
                temp->data.phonum,
                temp->data.code,
                temp->data.name,
                temp->data.idnumber,
                temp->data.sex,
                temp->data.note,
                temp->data.numberofflight,
                temp->data.points);
            temp = temp->next;
        }

        fclose(file);

        while (getchar() != '\n'); // 清空输入缓冲区
    }

    printf("信息修改完成\n");
    save_customers(head);
}

// 用户操作菜单函数
void user_operation(struct cuslist* current, struct cuslist* head) {
    // 直接使用全局变量
    struct Flightlist* flightHead = global_flight_head;

    while (1) {
        // 显示操作菜单选项
        printf("1. 修改用户信息\n");
        printf("2. 修改登机信息\n");
        printf("3. 查询航班信息\n");
        printf("4. 购买机票\n");
        printf("5. 模糊查询预订记录\n");
        printf("6. 查看购票记录\n");
        printf("0. 退出登录\n");
        int choice;
        scanf("%d", &choice);
        char flightNumber[10];
        const char* flight_Number;
        if (choice == 0) {
            break;
        }
        switch (choice) {
        case 1:
            reviseinformation(current, head);
            break;
        case 2:
            int key = reflightinformation(current, head);
            if (key == 0) {
                break;
            }
            break;
        case 3:
            query_flight(flightHead);
            break;
        case 4:
            buy(flightHead, &current->data);
            break;
        case 5:
            fuzzy_search_bookings(&current->data);
            break;
        case 6:
            flightRecord(&current->data);
            break;
        default:
            printf("无效的选择，请重新输入！\n");
        }
    }
}

// 检查链表是否存在循环的函数
int has_cycle(struct Flightlist* head) {
    struct Flightlist* slow = head;
    struct Flightlist* fast = head;

    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;

        if (slow == fast) {
            return 1; // 存在循环
        }
    }
    return 0; // 不存在循环
}

// 释放用户信息链表内存的函数
void free_cuslist(struct cuslist* head) {
    struct cuslist* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}
