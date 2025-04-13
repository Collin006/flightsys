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

// ���������������
const char* cities[] = {
    "����", "�Ϻ�", "����", "����", "�ɶ�", "����", "����", "�Ͼ�", "�人",
    "����", "��ɳ", "�ൺ", "����", "���", "����", "����", "����", "������",
    "��³ľ��", "����", "����", "����", "���ͺ���", "����", "����", "����",
    "����", "�ϲ�", "�Ϸ�", "ʯ��ׯ", "̫ԭ", "����", "�麣", "����", "����",
    "����", "�żҽ�", "��ɽ", "����ɽ", "����ɽ", "��ʲ", "�����", "������",
    "Ȫ��", "����", "����", "����", "��ͨ", "����", "��̨", "����", "Ϋ��",
    "��ɽ", "����", "տ��", "��ͷ", "����", "��˫����", "����", "â��",
    "����", "��ɽ", "�ն�", "�ٲ�", "��ͨ", "��ɽ", "����", "���", "��Ϫ",
    "����", "ŭ��", "�º�", "���", "����", "̨��", "����"
};

// ���о�γ����Ϣ
const double city_coordinates[][2] = {
    {39.9042, 116.4074},  // ����
    {31.2304, 121.4737},  // �Ϻ�
    {23.1291, 113.2644},  // ����
    {22.5431, 114.0579},  // ����
    {30.5728, 104.0668},  // �ɶ�
    {29.5631, 106.5504},  // ����
    {30.2741, 120.1551},  // ����
    {32.0415, 118.7674},  // �Ͼ�
    {30.5928, 114.3055},  // �人
    {34.2632, 108.9487},  // ����
    {28.1940, 112.9822},  // ��ɳ
    {36.0700, 120.3852},  // �ൺ
    {24.4642, 118.0886},  // ����
    {39.0842, 117.2006},  // ���
    {25.0406, 102.7122},  // ����
    {41.8056, 123.4290},  // ����
    {38.9140, 121.6147},  // ����
    {45.7548, 126.6286},  // ������
    {43.8281, 87.6298},   // ��³ľ��
    {36.6208, 101.7789},  // ����
    {36.0581, 103.8343},  // ����
    {38.4864, 106.2781},  // ����
    {40.8425, 111.7414},  // ���ͺ���
    {22.8455, 108.2940},  // ����
    {26.5784, 106.7134},  // ����
    {26.0753, 119.3062},  // ����
    {20.0160, 110.3322},  // ����
    {28.6764, 115.8617},  // �ϲ�
    {31.8611, 117.2831},  // �Ϸ�
    {38.0422, 114.5149},  // ʯ��ׯ
    {37.8732, 112.5492},  // ̫ԭ
    {29.6603, 91.1322},   // ����
    {22.2577, 113.5439},  // �麣
    {18.2522, 109.5281},  // ����
    {25.2852, 110.2982},  // ����
    {26.8624, 100.2468},  // ����
    {29.1267, 110.4792},  // �żҽ�
    {30.1602, 118.2973},  // ��ɽ
    {27.7078, 118.0327},  // ����ɽ
    {42.0000, 127.5000},  // ����ɽ������ֵ��
    {39.4778, 75.9964},   // ��ʲ
    {41.6978, 86.1072},   // �����
    {49.5883, 117.4383},  // ������
    {24.9062, 118.6072},  // Ȫ��
    {28.0020, 120.6567},  // ����
    {29.8683, 121.5440},  // ����
    {31.7772, 119.9665},  // ����
    {32.0119, 120.8647},  // ��ͨ
    {34.2617, 117.1946},  // ����
    {37.5025, 121.3960},  // ��̨
    {37.5073, 122.1174},  // ����
    {36.7010, 119.1332},  // Ϋ��
    {23.0225, 113.1227},  // ��ɽ
    {23.5411, 116.3773},  // ����
    {21.2717, 110.3581},  // տ��
    {23.3964, 116.6321},  // ��ͷ
    {27.6937, 106.9193},  // ����
    {21.9550, 101.9983},  // ��˫����
    {25.6049, 100.2367},  // ����
    {24.4058, 98.5427},   // â��
    {27.8333, 99.8667},   // ����
    {25.1211, 99.1709},   // ��ɽ
    {22.7731, 101.0122},  // �ն�
    {23.8831, 100.0918},  // �ٲ�
    {27.3309, 103.7022},  // ��ͨ
    {23.3775, 104.3469},  // ��ɽ
    {25.0304, 101.5406},  // ����
    {23.3500, 103.3900},  // ���
    {24.3558, 102.5469},  // ��Ϫ
    {25.5078, 103.7936},  // ����
    {26.8800, 98.8500},   // ŭ��
    {24.4300, 98.5900},   // �º�
    {22.3193, 114.1694},  // ���
    {22.1987, 113.5439},  // ����
    {25.0330, 121.5654},  // ̨��
    {22.6273, 120.3014}   // ����
};

// ��������ʡ��
const char* provinces[] = {
    "����", "�Ϻ�", "�㶫", "�㶫", "�Ĵ�", "����", "�㽭", "����", "����",
    "����", "����", "ɽ��", "����", "���", "����", "����", "����", "������",
    "�½�", "�ຣ", "����", "����", "���ɹ�", "����", "����", "����",
    "����", "����", "����", "�ӱ�", "ɽ��", "����", "�㶫", "����", "����",
    "����", "����", "����", "����", "����", "�½�", "�½�", "���ɹ�",
    "����", "�㽭", "�㽭", "����", "����", "����", "ɽ��", "ɽ��", "ɽ��",
    "�㶫", "�㶫", "�㶫", "�㶫", "����", "����", "����", "����",
    "����", "����", "����", "����", "����", "����", "����", "����", "����",
    "����", "����", "����", "���", "����", "̨��", "̨��"
};

// ���з�չˮƽ����
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


// Ŀ�ĵ��ȶ�
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

// �ɻ����ͼ�ά���ɱ�ϵ��
const double aircraft_types[][2] = {
    {1, 1.0}, // ��ͨ�ɻ���ά���ɱ�ϵ�� 1.0
    {2, 1.2}, // ���Ϳ���ͻ���ά���ɱ�ϵ�� 1.2
    {3, 0.8}  // С�Ϳͻ���ά���ɱ�ϵ�� 0.8
};

// ����ڼ��գ�������Ҫ�ڼ��գ�
const int holidays[][2] = {
    {1, 1},    // Ԫ��
    {2, 10},   // ����
    {4, 5},    // ������
    {5, 1},    // �Ͷ���
    {6, 22},   // �����
    {9, 29},   // �����
    {10, 1}    // �����
};

// �����������λ��Ϣ
void generate_seat_info(int* total_seats, int* seat_rows, int* seat_cols, int* first_class_seats, int* business_class_seats, int* economy_class_seats) {
    int possible_rows[] = { 20, 25, 30, 35, 40 };
    int possible_cols[] = { 4, 5, 6, 7, 8 };
    int row_index = rand() % 5;
    int col_index = rand() % 5;
    *seat_rows = possible_rows[row_index];
    *seat_cols = possible_cols[col_index];
    *total_seats = *seat_rows * *seat_cols;

    // ����ÿ����λ����������֤��������������
    int total = *total_seats;
    *first_class_seats = (*seat_cols) * ((int)(total * 0.1) / (*seat_cols));
    *business_class_seats = (*seat_cols) * ((int)(total * 0.2) / (*seat_cols));
    *economy_class_seats = total - *first_class_seats - *business_class_seats;
}

// ����������γ��֮��ľ���
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

// ������м����
double calculate_distance(int from_index, int to_index) {
    double lat1 = city_coordinates[from_index][0];
    double lon1 = city_coordinates[from_index][1];
    double lat2 = city_coordinates[to_index][0];
    double lon2 = city_coordinates[to_index][1];
    return haversine_distance(lat1, lon1, lat2, lon2);
}

// �ж��Ƿ�Ϊ�ڼ���
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

    // ���ݾ��뷶Χ���ò�ͬ�Ļ���Ʊ��ϵ��
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

    // ���ǳ��еȼ�����Ʊ��
    if (city_class[from_index] == 1 && city_class[to_index] == 1) {
        base_price *= 1.2 + (float)rand() / RAND_MAX * 0.1;
    }
    else if (city_class[from_index] == 1 || city_class[to_index] == 1) {
        base_price *= 1.1 + (float)rand() / RAND_MAX * 0.05;
    }
    else if (city_class[from_index] == 2 || city_class[to_index] == 2) {
        base_price *= 1.05 + (float)rand() / RAND_MAX * 0.03;
    }

    // ����Ŀ�ĵ��ȶȵ���Ʊ��
    base_price *= destination_popularity[to_index];

    // �ڼ����Ҹ��ݳ��еȼ��Ӽۣ��ʶ��ϵ��ڼ���Ʊ��
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

    // ���Ƿ���ʱ��ͺ���ʱ�̵���Ʊ��
    if (departure_hour >= 6 && departure_hour < 9) { // ��߷�
        base_price *= 1.03 + (float)rand() / RAND_MAX * 0.02;
    }
    else if (departure_hour >= 17 && departure_hour < 20) { // ��߷�
        base_price *= 1.03 + (float)rand() / RAND_MAX * 0.02;
    }
    else if (departure_hour >= 0 && departure_hour < 6) { // ���ۺ���
        base_price *= 0.7 + (float)rand() / RAND_MAX * 0.05;
    }

    // ���Ƿɻ�������ά���ɱ�
    int aircraft_type_index = rand() % 3;
    base_price *= aircraft_types[aircraft_type_index][1];

    // ȷ�����ò�Ʊ�۲����� 300 Ԫ�������� 5000 Ԫ
    if (base_price < 300) {
        base_price = 300;
    }
    else if (base_price > 5000) {
        base_price = 5000;
    }

    return base_price;
}

// ����Ʊ��
void calculate_prices(int from_index, int to_index, int month, int day, int departure_hour, float* first_class_price, float* business_class_price, float* economy_class_price) {
    float base_price = calculate_base_price(from_index, to_index, month, day, departure_hour);

    // �Ի����۸�����������뵽��ӽ�������
    base_price = (float)round(base_price);

    *first_class_price = base_price * 2.0f;
    *business_class_price = base_price * 1.5f;
    *economy_class_price = base_price;

    // �Ը���λ�۸�����������뵽��ӽ�������
    *first_class_price = (float)round(*first_class_price);
    *business_class_price = (float)round(*business_class_price);
    *economy_class_price = (float)round(*economy_class_price);
}

// �����ٶȣ�����/Сʱ��
const double FLIGHT_SPEED = 800;

// �������ʱ��
void generate_random_time(int year, int month, int day, int from_index, int to_index, char* departure_time, char* arrival_time, int* departure_hour) {
    *departure_hour = (rand() % 18) + 6; // 6�㵽24��֮�����
    int departure_minute = rand() % 60;

    // ��������֮��ľ���
    double distance = calculate_distance(from_index, to_index);
    // ���ݾ���������ʱ�䣨Сʱ����������һ���Ĳ�����Χ
    double flight_duration_hours = distance / FLIGHT_SPEED + (rand() % 60 - 30) / 60.0;
    if (flight_duration_hours < 1) {
        flight_duration_hours = 1; // ��̷���ʱ��Ϊ1Сʱ
    }

    int flight_duration = (int)(flight_duration_hours * 60); // ת��Ϊ����

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

// �洢�����ɵĺ����ż���λ��Ϣ
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

// ���ɺ����ż���λ��Ϣ
void generate_flight_number_and_seats(int from_index, int to_index, int day, int flight_count, int departure_hour, char* flight_number,
    int* total_seats, int* seat_rows, int* seat_cols, int* first_class_seats, int* business_class_seats, int* economy_class_seats) {
    for (int i = 0; i < record_count; i++) {
        if (records[i].from_index == from_index && records[i].to_index == to_index && records[i].seat_rows == *seat_rows && records[i].seat_cols == *seat_cols) {
            strcpy(flight_number, records[i].flight_number);
            return;
        }
    }

    int unique_id = (from_index * 100 + to_index + rand() % 9999) % 9999; // ���ݳ����ء�Ŀ�ĵغ����������Ψһ���
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

// ����Ҫ���˵ĳ�������
const char* filtered_cities[] = {
    "�����", "����ɽ", "��ɽ", "��ͨ", "��ɽ", "����", "���", "��Ϫ",
    "����", "����", "��ɽ", "տ��", "�º�", "����"
};

// �������Ƿ���Ҫ����
int is_city_filtered(const char* city) {
    int num_filtered = sizeof(filtered_cities) / sizeof(filtered_cities[0]);
    for (int i = 0; i < num_filtered; i++) {
        if (strcmp(city, filtered_cities[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// ���ɺ�����Ϣ
void generate_flight_info(int year, int month, int day, FILE* file) {
    if (file == NULL) {
        return;
    }
    int num_cities = sizeof(cities) / sizeof(cities[0]);
    for (int i = 0; i < num_cities; i++) {
        // �����������Ƿ���Ҫ����
        if (is_city_filtered(cities[i])) {
            continue;
        }
        for (int j = 0; j < num_cities; j++) {
            if (i != j && strcmp(provinces[i], provinces[j]) != 0) {
                // ��鵽������Ƿ���Ҫ����
                if (is_city_filtered(cities[j])) {
                    continue;
                }
                // ÿ�����ж�֮������1��2������
                for (int flight_count = 0; flight_count < (rand() % 2 + 1); flight_count++) {
                    char departure_time[30];
                    char arrival_time[30];
                    char flight_number[10];
                    float first_class_price, business_class_price, economy_class_price;
                    int total_seats, seat_rows, seat_cols, first_class_seats, business_class_seats, economy_class_seats;
                    int departure_hour;

                    // ���ɳ����͵���ʱ��
                    generate_random_time(year, month, day, i, j, departure_time, arrival_time, &departure_hour);
                    // ���ɺ���ź���λ��Ϣ
                    generate_flight_number_and_seats(i, j, day, flight_count, departure_hour, flight_number,
                        &total_seats, &seat_rows, &seat_cols, &first_class_seats, &business_class_seats, &economy_class_seats);
                    // ����Ʊ��
                    calculate_prices(i, j, month, day, departure_hour, &first_class_price, &business_class_price, &economy_class_price);

                    // ʹ�ö��ŷָ������Ϣ
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

// ���� 2025 ��һ��ĺ�����Ϣ
void generate_yearly_flight_info() {
    FILE* file = fopen("flights.txt", "w");
    if (file == NULL) {
        perror("�޷����ļ�");
        return;
    }

    srand((unsigned int)time(NULL));

    int days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    for (int month = 1; month <= 12; month++) {
        int days = days_in_month[month - 1];
        if (month == 2 && ((2025 % 4 == 0 && 2025 % 100 != 0) || (2025 % 400 == 0))) {
            days = 29; // ���� 2 ���� 29 ��
        }
        for (int day = 1; day <= days; day++) {
            generate_flight_info(2025, month, day, file);
        }
    }

    fclose(file);
}

