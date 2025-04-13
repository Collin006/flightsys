#define _CRT_SECURE_NO_WARNINGS
#define NUM_HOLIDAYS (sizeof(holidays) / sizeof(holidays[0]))
#define M_PI 3.14159265358979323846
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "structure.h"

extern void save_flights(struct Flightlist* head);

// 定义城市名称数组
const char* cities[] = {
    "北京", "上海", "广州", "深圳", "成都", "重庆", "杭州", "南京", "武汉",
    "西安", "长沙", "青岛", "厦门", "天津", "昆明", "沈阳", "大连", "哈尔滨",
    "乌鲁木齐", "西宁", "兰州", "银川", "呼和浩特", "南宁", "贵阳", "福州",
    "海口", "南昌", "合肥", "石家庄", "太原", "拉萨", "珠海", "三亚", "桂林",
    "丽江", "张家界", "黄山", "武夷山", "长白山", "喀什", "库尔勒", "满洲里",
    "泉州", "温州", "宁波", "常州", "南通", "徐州", "烟台", "威海", "潍坊",
    "佛山", "揭阳", "湛江", "汕头", "遵义", "西双版纳", "大理", "芒市",
    "迪庆", "保山", "普洱", "临沧", "昭通", "文山", "楚雄", "红河", "玉溪",
    "曲靖", "怒江", "德宏", "香港", "澳门", "台北", "高雄"
};

// 城市经纬度信息
const double city_coordinates[][2] = {
    {39.9042, 116.4074},  // 北京
    {31.2304, 121.4737},  // 上海
    {23.1291, 113.2644},  // 广州
    {22.5431, 114.0579},  // 深圳
    {30.5728, 104.0668},  // 成都
    {29.5631, 106.5504},  // 重庆
    {30.2741, 120.1551},  // 杭州
    {32.0415, 118.7674},  // 南京
    {30.5928, 114.3055},  // 武汉
    {34.2632, 108.9487},  // 西安
    {28.1940, 112.9822},  // 长沙
    {36.0700, 120.3852},  // 青岛
    {24.4642, 118.0886},  // 厦门
    {39.0842, 117.2006},  // 天津
    {25.0406, 102.7122},  // 昆明
    {41.8056, 123.4290},  // 沈阳
    {38.9140, 121.6147},  // 大连
    {45.7548, 126.6286},  // 哈尔滨
    {43.8281, 87.6298},   // 乌鲁木齐
    {36.6208, 101.7789},  // 西宁
    {36.0581, 103.8343},  // 兰州
    {38.4864, 106.2781},  // 银川
    {40.8425, 111.7414},  // 呼和浩特
    {22.8455, 108.2940},  // 南宁
    {26.5784, 106.7134},  // 贵阳
    {26.0753, 119.3062},  // 福州
    {20.0160, 110.3322},  // 海口
    {28.6764, 115.8617},  // 南昌
    {31.8611, 117.2831},  // 合肥
    {38.0422, 114.5149},  // 石家庄
    {37.8732, 112.5492},  // 太原
    {29.6603, 91.1322},   // 拉萨
    {22.2577, 113.5439},  // 珠海
    {18.2522, 109.5281},  // 三亚
    {25.2852, 110.2982},  // 桂林
    {26.8624, 100.2468},  // 丽江
    {29.1267, 110.4792},  // 张家界
    {30.1602, 118.2973},  // 黄山
    {27.7078, 118.0327},  // 武夷山
    {42.0000, 127.5000},  // 长白山（近似值）
    {39.4778, 75.9964},   // 喀什
    {41.6978, 86.1072},   // 库尔勒
    {49.5883, 117.4383},  // 满洲里
    {24.9062, 118.6072},  // 泉州
    {28.0020, 120.6567},  // 温州
    {29.8683, 121.5440},  // 宁波
    {31.7772, 119.9665},  // 常州
    {32.0119, 120.8647},  // 南通
    {34.2617, 117.1946},  // 徐州
    {37.5025, 121.3960},  // 烟台
    {37.5073, 122.1174},  // 威海
    {36.7010, 119.1332},  // 潍坊
    {23.0225, 113.1227},  // 佛山
    {23.5411, 116.3773},  // 揭阳
    {21.2717, 110.3581},  // 湛江
    {23.3964, 116.6321},  // 汕头
    {27.6937, 106.9193},  // 遵义
    {21.9550, 101.9983},  // 西双版纳
    {25.6049, 100.2367},  // 大理
    {24.4058, 98.5427},   // 芒市
    {27.8333, 99.8667},   // 迪庆
    {25.1211, 99.1709},   // 保山
    {22.7731, 101.0122},  // 普洱
    {23.8831, 100.0918},  // 临沧
    {27.3309, 103.7022},  // 昭通
    {23.3775, 104.3469},  // 文山
    {25.0304, 101.5406},  // 楚雄
    {23.3500, 103.3900},  // 红河
    {24.3558, 102.5469},  // 玉溪
    {25.5078, 103.7936},  // 曲靖
    {26.8800, 98.8500},   // 怒江
    {24.4300, 98.5900},   // 德宏
    {22.3193, 114.1694},  // 香港
    {22.1987, 113.5439},  // 澳门
    {25.0330, 121.5654},  // 台北
    {22.6273, 120.3014}   // 高雄
};

// 城市所属省份
const char* provinces[] = {
    "北京", "上海", "广东", "广东", "四川", "重庆", "浙江", "江苏", "湖北",
    "陕西", "湖南", "山东", "福建", "天津", "云南", "辽宁", "辽宁", "黑龙江",
    "新疆", "青海", "甘肃", "宁夏", "内蒙古", "广西", "贵州", "福建",
    "海南", "江西", "安徽", "河北", "山西", "西藏", "广东", "海南", "广西",
    "云南", "湖南", "安徽", "福建", "吉林", "新疆", "新疆", "内蒙古",
    "福建", "浙江", "浙江", "江苏", "江苏", "江苏", "山东", "山东", "山东",
    "广东", "广东", "广东", "广东", "贵州", "云南", "云南", "云南",
    "云南", "云南", "云南", "云南", "云南", "云南", "云南", "云南", "云南",
    "云南", "云南", "云南", "香港", "澳门", "台湾", "台湾"
};

// 城市发展水平分类
const int city_class[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 2, 2, 2, 1, 2, 2, 2, 2,
    2, 3, 3, 3, 3, 2, 2, 2,
    2, 2, 2, 2, 2, 3, 2, 1, 2,
    2, 2, 2, 2, 2, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 1, 1, 1, 1
};


// 目的地热度
const double destination_popularity[100] = {
    1.2, 1.2, 1.2, 1.2, 1.1, 1.1, 1.1, 1.1, 1.1,
    1.1, 1.0, 1.0, 1.0, 1.1, 1.0, 1.0, 1.0, 1.0,
    0.9, 0.9, 0.9, 0.9, 0.9, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 0.8, 1.1, 1.3, 1.1,
    1.1, 1.1, 1.1, 1.1, 1.1, 0.9, 0.9, 0.9,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.1, 1.1, 1.1, 1.1, 1.0, 1.1, 1.1, 1.1,
    1.1, 1.1, 1.1, 1.1, 1.1, 1.1, 1.1, 1.1,
    1.1, 1.1, 1.1, 1.1, 1.2, 1.2, 1.2, 1.2
};

// 飞机类型及维护成本系数
const double aircraft_types[][2] = {
    {1, 1.0}, // 普通飞机，维护成本系数 1.0
    {2, 1.2}, // 大型宽体客机，维护成本系数 1.2
    {3, 0.8}  // 小型客机，维护成本系数 0.8
};

// 定义节假日（部分主要节假日）
const int holidays[][2] = {
    {1, 1},    // 元旦
    {2, 10},   // 春节
    {4, 5},    // 清明节
    {5, 1},    // 劳动节
    {6, 22},   // 端午节
    {9, 29},   // 中秋节
    {10, 1}    // 国庆节
};

// 生成随机的座位信息
void generate_seat_info(int* total_seats, int* seat_rows, int* seat_cols, int* first_class_seats, int* business_class_seats, int* economy_class_seats) {
    int possible_rows[] = { 20, 25, 30, 35, 40 };
    int possible_cols[] = { 4, 5, 6, 7, 8 };
    int row_index = rand() % 5;
    int col_index = rand() % 5;
    *seat_rows = possible_rows[row_index];
    *seat_cols = possible_cols[col_index];
    *total_seats = *seat_rows * *seat_cols;

    // 计算每种座位的数量，保证是列数的整数倍
    int total = *total_seats;
    *first_class_seats = (*seat_cols) * ((int)(total * 0.1) / (*seat_cols));
    *business_class_seats = (*seat_cols) * ((int)(total * 0.2) / (*seat_cols));
    *economy_class_seats = total - *first_class_seats - *business_class_seats;
}

// 计算两个经纬度之间的距离
double haversine_distance(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371;
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;
    lat1 = lat1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;

    double a = sin(dLat / 2) * sin(dLat / 2) +
        sin(dLon / 2) * sin(dLon / 2) * cos(lat1) * cos(lat2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return R * c;
}

// 计算城市间距离
double calculate_distance(int from_index, int to_index) {
    double lat1 = city_coordinates[from_index][0];
    double lon1 = city_coordinates[from_index][1];
    double lat2 = city_coordinates[to_index][0];
    double lon2 = city_coordinates[to_index][1];
    return haversine_distance(lat1, lon1, lat2, lon2);
}

// 判断是否为节假日
int is_holiday(int month, int day) {
    for (int i = 0; i < NUM_HOLIDAYS; i++) {
        if (holidays[i][0] == month && holidays[i][1] == day) {
            return 1;
        }
    }
    return 0;
}

float calculate_base_price(int from_index, int to_index, int month, int day, int departure_hour) {
    double distance = calculate_distance(from_index, to_index);
    float base_price;

    // 根据距离范围设置不同的基础票价系数
    if (distance < 500) {
        base_price = (float)(distance * (0.2 + (float)rand() / RAND_MAX * 0.05));
    }
    else if (distance < 1500) {
        base_price = (float)(distance * (0.18 + (float)rand() / RAND_MAX * 0.1));
    }
    else if (distance < 3000) {
        base_price = (float)(distance * (0.15 + (float)rand() / RAND_MAX * 0.05));
    }
    else {
        base_price = (float)(distance * (0.12 + (float)rand() / RAND_MAX * 0.02));
    }

    // 考虑城市等级调整票价
    if (city_class[from_index] == 1 && city_class[to_index] == 1) {
        base_price *= 1.2 + (float)rand() / RAND_MAX * 0.1;
    }
    else if (city_class[from_index] == 1 || city_class[to_index] == 1) {
        base_price *= 1.1 + (float)rand() / RAND_MAX * 0.05;
    }
    else if (city_class[from_index] == 2 || city_class[to_index] == 2) {
        base_price *= 1.05 + (float)rand() / RAND_MAX * 0.03;
    }

    // 考虑目的地热度调整票价
    base_price *= destination_popularity[to_index];

    // 节假日且根据城市等级加价，适度上调节假日票价
    if (is_holiday(month, day)) {
        float holiday_multiplier = 1.0;
        if (city_class[from_index] == 1 && city_class[to_index] == 1) {
            holiday_multiplier = 1.8 + (float)rand() / RAND_MAX * 0.2;
        }
        else if (city_class[from_index] == 1 || city_class[to_index] == 1) {
            holiday_multiplier = 1.6 + (float)rand() / RAND_MAX * 0.15;
        }
        else {
            holiday_multiplier = 1.4 + (float)rand() / RAND_MAX * 0.1;
        }
    }

    // 考虑飞行时间和航班时刻调整票价
    if (departure_hour >= 6 && departure_hour < 9) { // 早高峰
        base_price *= 1.03 + (float)rand() / RAND_MAX * 0.02;
    }
    else if (departure_hour >= 17 && departure_hour < 20) { // 晚高峰
        base_price *= 1.03 + (float)rand() / RAND_MAX * 0.02;
    }
    else if (departure_hour >= 0 && departure_hour < 6) { // 红眼航班
        base_price *= 0.7 + (float)rand() / RAND_MAX * 0.05;
    }

    // 考虑飞机购置与维护成本
    int aircraft_type_index = rand() % 3;
    base_price *= aircraft_types[aircraft_type_index][1];

    // 确保经济舱票价不低于 300 元，不超过 5000 元
    if (base_price < 300) {
        base_price = 300;
    }
    else if (base_price > 5000) {
        base_price = 5000;
    }

    return base_price;
}

// 计算票价
void calculate_prices(int from_index, int to_index, int month, int day, int departure_hour, float* first_class_price, float* business_class_price, float* economy_class_price) {
    float base_price = calculate_base_price(from_index, to_index, month, day, departure_hour);

    // 对基础价格进行四舍五入到最接近的整数
    base_price = (float)round(base_price);

    *first_class_price = base_price * 2.0f;
    *business_class_price = base_price * 1.5f;
    *economy_class_price = base_price;

    // 对各舱位价格进行四舍五入到最接近的整数
    *first_class_price = (float)round(*first_class_price);
    *business_class_price = (float)round(*business_class_price);
    *economy_class_price = (float)round(*economy_class_price);
}

// 飞行速度（公里/小时）
const double FLIGHT_SPEED = 800;

// 生成随机时间
void generate_random_time(int year, int month, int day, int from_index, int to_index, char* departure_time, char* arrival_time, int* departure_hour) {
    *departure_hour = (rand() % 18) + 6; // 6点到24点之间随机
    int departure_minute = rand() % 60;

    // 计算两地之间的距离
    double distance = calculate_distance(from_index, to_index);
    // 根据距离估算飞行时间（小时），并考虑一定的波动范围
    double flight_duration_hours = distance / FLIGHT_SPEED + (rand() % 60 - 30) / 60.0;
    if (flight_duration_hours < 1) {
        flight_duration_hours = 1; // 最短飞行时间为1小时
    }

    int flight_duration = (int)(flight_duration_hours * 60); // 转换为分钟

    struct tm departure_tm = { 0 };
    departure_tm.tm_year = year - 1900;
    departure_tm.tm_mon = month - 1;
    departure_tm.tm_mday = day;
    departure_tm.tm_hour = *departure_hour;
    departure_tm.tm_min = departure_minute;
    departure_tm.tm_sec = 0;

    time_t departure_time_t = mktime(&departure_tm);
    time_t arrival_time_t = departure_time_t + flight_duration * 60;

    struct tm* arrival_tm = localtime(&arrival_time_t);

    sprintf(departure_time, "%04d-%02d-%02d %02d:%02d:00", year, month, day, *departure_hour, departure_minute);
    sprintf(arrival_time, "%04d-%02d-%02d %02d:%02d:00", arrival_tm->tm_year + 1900, arrival_tm->tm_mon + 1, arrival_tm->tm_mday, arrival_tm->tm_hour, arrival_tm->tm_min);
}

// 存储已生成的航班编号及座位信息
typedef struct {
    int from_index;
    int to_index;
    int seat_rows;
    int seat_cols;
    char flight_number[10];
} FlightRecord;

#define MAX_RECORDS 10000
FlightRecord records[MAX_RECORDS];
int record_count = 0;

// 生成航班编号及座位信息
void generate_flight_number_and_seats(int from_index, int to_index, int day, int flight_count, int departure_hour, char* flight_number,
    int* total_seats, int* seat_rows, int* seat_cols, int* first_class_seats, int* business_class_seats, int* economy_class_seats) {
    for (int i = 0; i < record_count; i++) {
        if (records[i].from_index == from_index && records[i].to_index == to_index && records[i].seat_rows == *seat_rows && records[i].seat_cols == *seat_cols) {
            strcpy(flight_number, records[i].flight_number);
            return;
        }
    }

    int unique_id = (from_index * 100 + to_index + rand() % 9999) % 9999; // 根据出发地、目的地和随机数生成唯一编号
    sprintf(flight_number, "CZ%04d", unique_id);

    generate_seat_info(total_seats, seat_rows, seat_cols, first_class_seats, business_class_seats, economy_class_seats);

    if (record_count < MAX_RECORDS) {
        records[record_count].from_index = from_index;
        records[record_count].to_index = to_index;
        records[record_count].seat_rows = *seat_rows;
        records[record_count].seat_cols = *seat_cols;
        strcpy(records[record_count].flight_number, flight_number);
        record_count++;
    }
}

// 定义要过滤的城市数组
const char* filtered_cities[] = {
    "库尔勒", "武夷山", "黄山", "昭通", "文山", "楚雄", "红河", "玉溪",
    "曲靖", "迪庆", "保山", "湛江", "德宏", "高雄"
};

// 检查城市是否需要过滤
int is_city_filtered(const char* city) {
    int num_filtered = sizeof(filtered_cities) / sizeof(filtered_cities[0]);
    for (int i = 0; i < num_filtered; i++) {
        if (strcmp(city, filtered_cities[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// 生成航班信息
void generate_flight_info(int year, int month, int day, FILE* file) {
    if (file == NULL) {
        return;
    }
    int num_cities = sizeof(cities) / sizeof(cities[0]);
    for (int i = 0; i < num_cities; i++) {
        // 检查出发城市是否需要过滤
        if (is_city_filtered(cities[i])) {
            continue;
        }
        for (int j = 0; j < num_cities; j++) {
            if (i != j && strcmp(provinces[i], provinces[j]) != 0) {
                // 检查到达城市是否需要过滤
                if (is_city_filtered(cities[j])) {
                    continue;
                }
                // 每个城市对之间生成1到2个航班
                for (int flight_count = 0; flight_count < (rand() % 2 + 1); flight_count++) {
                    char departure_time[30];
                    char arrival_time[30];
                    char flight_number[10];
                    float first_class_price, business_class_price, economy_class_price;
                    int total_seats, seat_rows, seat_cols, first_class_seats, business_class_seats, economy_class_seats;
                    int departure_hour;

                    // 生成出发和到达时间
                    generate_random_time(year, month, day, i, j, departure_time, arrival_time, &departure_hour);
                    // 生成航班号和座位信息
                    generate_flight_number_and_seats(i, j, day, flight_count, departure_hour, flight_number,
                        &total_seats, &seat_rows, &seat_cols, &first_class_seats, &business_class_seats, &economy_class_seats);
                    // 计算票价
                    calculate_prices(i, j, month, day, departure_hour, &first_class_price, &business_class_price, &economy_class_price);

                    // 使用逗号分隔输出信息
                    fprintf(file, "%s,%s,%s,%s,%s,%d,%d,%d,%d,%d,%.2f,%d,%d,%.2f,%d,%d,%.2f\n",
                        cities[i], cities[j], departure_time, arrival_time, flight_number,
                        total_seats, seat_rows, seat_cols,
                        first_class_seats, first_class_seats, first_class_price,
                        business_class_seats, business_class_seats, business_class_price,
                        economy_class_seats, economy_class_seats, economy_class_price);
                }
            }
        }
    }
}

// 生成 2025 年一年的航班信息
void generate_yearly_flight_info() {
    FILE* file = fopen("flights.txt", "w");
    if (file == NULL) {
        perror("无法打开文件");
        return;
    }

    srand((unsigned int)time(NULL));

    int days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    for (int month = 1; month <= 12; month++) {
        int days = days_in_month[month - 1];
        if (month == 2 && ((2025 % 4 == 0 && 2025 % 100 != 0) || (2025 % 400 == 0))) {
            days = 29; // 闰年 2 月有 29 天
        }
        for (int day = 1; day <= days; day++) {
            generate_flight_info(2025, month, day, file);
        }
    }

    fclose(file);
}

