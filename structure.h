#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>

// �û���Ϣ�ṹ��
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

// ����ṹ��
struct Flight {
    char pdeparture[20];    // �����أ���Ҫ��Ӧ��������
    char pdestination[20];  // Ŀ�ĵأ���Ҫ��Ӧ��������
    char tdaparture[30];    // ����ʱ�䣬��Ҫ��Ӧ��׼��ʱ���ʽ
    char tdestination[30];  // ����ʱ�䣬��Ҫ��Ӧ��׼��ʱ���ʽ
    char number[10];        // ������
    int tnumber;            // ����λ��
    int seat_rows;          // ��λ����
    int seat_cols;          // ��λ����
    int first_class_num;    // ͷ�Ȳ���λ��
    int first_class_remain; // ͷ�Ȳ�ʣ����λ��
    float first_class_price;// ͷ�Ȳ�Ʊ��
    int business_class_num; // �������λ��
    int business_class_remain; // �����ʣ����λ��
    float business_class_price; // �����Ʊ��
    int economy_class_num;  // ���ò���λ��
    int economy_class_remain; // ���ò�ʣ����λ��
    float economy_class_price; // ���ò�Ʊ��
};

// ��������ṹ��
struct Flightlist
{
    struct Flight data;
    char time[9];
    char seatnum[4];
    struct Flightlist* next;
    char passenger_name[20];   // ��Ʊʱ��д�ĳ˿�����
    char passenger_id[20];     // ��Ʊʱ��д�����֤��
};

// �û�����ṹ��
struct cuslist
{
    struct cusman data;
    struct cuslist* next;
};

// ����һ���ṹ�����ڴ�ŵ�¼���
typedef struct {
    struct cuslist* current;
    struct cuslist* head;
} LoginResult;

// ���ļ��м����û���Ϣ
void loadcusman(struct cuslist** head);

// ע���û�
void register_customer();

// �û���¼
LoginResult login();

// ���ļ��м��غ�����Ϣ
void load_flights(struct Flightlist** head);

// ɾ��ָ������
void delete_flight(struct Flightlist** head, const char* flight_number);

// ��ʾʣ��ʱ��
void Display_remainingtime(long seconds);

// ���������û���Ϣ
struct cuslist* load_all_customers();

// ����ʣ��ʱ��
long Calculate_time(char* departure_timestamp);

// �����û���Ϣ
void load_users(struct cuslist** head);

// �Զ��岻���ִ�Сд��strstr����
int strcasestr(const char* haystack, const char* needle);

// �û�ѡ����λ
void chooseseat(struct Flightlist* current, int seatClass);

// �޸��û�����
void modify_password(struct cuslist* current, struct cuslist* head);

// ��ʼ��������Ϣ
void initFlight(struct Flight* flight);

// �û�ģ����ѯ�Լ��Ķ�Ʊ��Ϣ
void fuzzy_search_bookings(struct cusman* current_user);

// ��ȡ�û����������
void get_password(char* password, int max_length);

// �޸��û���Ϣ
void reviseinformation(struct cuslist* current_user, struct cuslist* head);

// ��Ӻ���
void add_flight(struct Flightlist** head);

// �޸ĺ�����Ϣ
void modify_flight(struct Flightlist* head, const char* flight_number);

// ��ѯ������Ϣ
void query_flight(struct Flightlist* head, const char* flight_number);

// �ͷź��������ڴ�
void free_flight_list(struct Flightlist* head);

// ����������
void flight_operation();

// �û�����
void user_operation(struct cuslist* current, struct cuslist* head);

// �û���Ʊ
void buy(struct Flightlist* head, struct cusman* customer);

// ��ѯ�û��ѹ���ĺ���
void query_purchased_flights(struct cusman* customer);

// �����к�����Ϣ���浽�ļ�
void save_flights(struct Flightlist* head);

// ��¼������Ϣ
void flightRecord(struct cusman* customer);

// �ͷ��û������ڴ�
void free_cuslist(struct cuslist* head);

// ������뻺����
void clear_input_buffer();

// ��ȡ��Ʊ��Ϣ���޸�
void read_purchase_info(struct Flightlist* newPurchased, struct cusman* customer, int seatClass);

// �����û��ѹ���ĺ�����Ϣ
void load_purchased_flights(struct cusman* customer);

// ��������Ƿ����ѭ��
int has_cycle(struct Flightlist* head);

// �ͷŵ�¼����ṹ��ռ�õ��ڴ�
void free_login_result(LoginResult result);