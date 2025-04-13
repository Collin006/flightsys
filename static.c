#include "structure.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_DATE_DIFF 7  // ������ڷ�Χ���ɸ���ʵ���������

// ����ȫ�ֱ�����ָ�򺽰��б��ͷָ��
extern struct Flightlist* global_flight_head;

// �Զ��� strptime ����
// �������ַ�������Ϊ struct tm �ṹ��
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

// �������ַ���ת��Ϊ time_t ����
time_t str_to_time(const char* date_str) {
    struct tm tm = { 0 };
    if (strptime(date_str, "%Y-%m-%d", &tm) == NULL) {
        fprintf(stderr, "���ڽ���ʧ��: %s\n", date_str);
        return -1;
    }
    return mktime(&tm);
}

// ���ļ��м��ؿͻ���Ϣ
void loadcusman(struct cuslist** head) {
    FILE* file = fopen("customers.txt", "r");
    if (file == NULL) {
        // ���ԭ�ļ������ڣ����Դӱ����ļ��м���
        file = fopen("customers_backup.txt", "r");
        if (file == NULL) {
            return;
        }
        printf("ԭ�û������ļ������ڣ��ӱ����ļ��м�������...\n");
    }

    struct cuslist* current = NULL;
    struct cuslist* new_customer;
    char line[1024];  // ���ڶ�ȡһ������

    // ���ж�ȡ�ļ�
    while (fgets(line, sizeof(line), file) != NULL) {
        new_customer = (struct cuslist*)malloc(sizeof(struct cuslist));
        if (new_customer == NULL) {
            printf("�ڴ����ʧ�ܣ�\n");
            fclose(file);
            return;
        }

        // ʹ�� sscanf ������ǰ��
        int fields_read = sscanf(line, "%11s %13s %19s %19s %9s %4s %d",
            new_customer->data.phonum, new_customer->data.code, new_customer->data.name,
            new_customer->data.idnumber, new_customer->data.note, new_customer->data.sex,
            &new_customer->data.points);

        if (fields_read != 7) {
            // ��ʽ��ƥ�䣬�ͷŵ�ǰ�ڵ�
            free(new_customer);
            continue;
        }

        new_customer->data.purchased_flights = NULL; // ��ʼ���ѹ��򺽰��б�
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

// ���ͻ���Ϣ���浽�ļ���ͬʱ���汸��
void save_customers(struct cuslist* head) {
    // ��ԭ�ļ�����д��
    FILE* file = fopen("customers.txt", "w");
    if (file == NULL) {
        perror("�޷��򿪿ͻ��ļ�����д��");
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

        // �����ѹ��򺽰���Ϣ
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

    // �򿪱����ļ�����д��
    FILE* backup_file = fopen("customers_backup.txt", "w");
    if (backup_file == NULL) {
        perror("�޷��򿪱����ļ�����д��");
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

        // �����ѹ��򺽰���Ϣ
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

// ���غ�����Ϣ
void load_flights() {
    if (global_flight_head != NULL) {
        return; // ���������Ϣ�Ѽ��أ�ֱ�ӷ���
    }

    FILE* file = fopen("flights.txt", "r");
    if (file == NULL) {
        perror("�޷��򿪺����ļ����ж�ȡ");
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
            printf("�ڴ����ʧ�ܣ�\n");
            fclose(file);
            return;
        }

        // ʹ�ö��ŷָ���������
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
    // ��ӡ���صĺ�����Ϣ�������ɸ�����Ҫ��Ӿ���ʵ��
}

// ��ȡ�û���������룬������������
void get_password(char* password, int max_length) {
    int i = 0;
    char ch;
    while (1) {
        ch = getch();  // ��ȡ�û����뵫����ʾ
        if (ch == '\r') {
            password[i] = '\0';  // �ַ���������
            printf("\n");
            break;
        }
        // ������˸���������룬ɾ��ǰһ���ַ�
        else if (ch == '\b' && i > 0) {
            i--;
            printf("\b \b");
        }
        // ����ǿɴ�ӡ�ַ���δ������󳤶ȣ���ӵ�������
        else if (i < max_length - 1 && (isprint(ch) || isspace(ch))) {
            password[i++] = ch;
            printf("*");
        }
    }
}

// �û�ע�Ṧ��
void register_customer() {
    struct cusman new_customer;
    struct cuslist* new_node = (struct cuslist*)malloc(sizeof(struct cuslist));
    if (new_node == NULL) {
        perror("�ڴ����ʧ��");
        return;
    }
    // ��ȡ�û�������ֻ�����
    while (1) {
        printf("�������ֻ�����: ");
        scanf("%11s", new_customer.phonum);
        while (getchar() != '\n'); // ��ջ�����
        if (strlen(new_customer.phonum) != 11) {
            printf("�ֻ����볤��ӦΪ11λ�����������룡\n");
            continue;
        }
        break;
    }
    // ��ȡ�û����������
    while (1) {
        printf("����������(����ӦΪ6λ�Ҳ�����12λ): ");
        get_password(new_customer.code, 13);
        if (strlen(new_customer.code) < 6 || strlen(new_customer.code) > 12) {
            printf("���볤��Ӧ��6��12λ֮�䣬���������룡\n");
            continue;
        }
        break;
    }
    // ��ȡ�û����������
    printf("����������: ");
    scanf("%19s", new_customer.name);
    while (getchar() != '\n'); // ��ջ�����
    // ��ȡ�û���������֤��
    while (1) {
        printf("���������֤��: ");
        scanf("%19s", new_customer.idnumber);
        while (getchar() != '\n'); // ��ջ�����
        if (strlen(new_customer.idnumber) != 18) {
            printf("���֤�ų���ӦΪ18λ�����������룡\n");
            continue;
        }
        break;
    }
    // ��ȡ�û�������Ա�
    printf("�������Ա�: ");
    scanf("%4s", new_customer.sex);
    while (getchar() != '\n'); // ��ջ�����
    // ѯ���û��Ƿ���Ҫ��д��ע
    printf("�Ƿ���Ҫ��д��עy or n\n");
    char keyword;
    scanf(" %c", &keyword);
    while (getchar() != '\n'); // ��ջ�����
    if (keyword == 'y' || keyword == 'Y') {
        printf("����д��ע:");
        scanf("%49s", new_customer.note);
        while (getchar() != '\n'); // ��ջ�����
    }
    else if (keyword == 'n' || keyword == 'N') {
        strcpy(new_customer.note, "��");
    }
    new_customer.purchased_flights = NULL;
    new_customer.numberofflight = 0;
    new_customer.points = 0;
    // ���¿ͻ���Ϣ��ֵ���½ڵ�
    new_node->data = new_customer;
    new_node->next = NULL;

    // ��׷��ģʽ���ļ�
    FILE* file = fopen("customers.txt", "a");
    if (file == NULL) {
        perror("�޷����ļ�");
        free(new_node);
        return;
    }
    // ���¿ͻ���Ϣд���ļ�
    fprintf(file, "%s %s %s %s %s %s %d %d\n",
        new_node->data.phonum,
        new_node->data.code,
        new_node->data.name,
        new_node->data.idnumber,
        new_node->data.sex,
        new_node->data.note,
        new_node->data.numberofflight,
        new_node->data.points);
    // �ر��ļ�
    fclose(file);

    // ���½ڵ���ӵ��ͻ��б�ͷ��
    struct cuslist* head = load_all_customers();
    new_node->next = head;
    head = new_node;

    // ������º���û���Ϣ
    save_customers(head);
}

// ���ļ��м����û���Ϣ
void load_users(struct cuslist** head) {
    FILE* file = fopen("customers.txt", "r");
    if (file == NULL) {
        printf("�޷����ļ�\n");
        return;
    }
    struct cuslist* current = NULL;
    char line[1024];
    // ���ж�ȡ�ļ�
    while (fgets(line, sizeof(line), file)) {
        struct cuslist* new_customer = (struct cuslist*)malloc(sizeof(struct cuslist));
        if (new_customer == NULL) {
            printf("�ڴ����ʧ��\n");
            fclose(file);
            return;
        }
        // �ָ�ÿ�е�����
        char* phonum = strtok(line, " ");
        char* code = strtok(NULL, " ");
        char* name = strtok(NULL, " ");
        char* idnumber = strtok(NULL, " ");
        char* sex = strtok(NULL, " ");
        char* note = strtok(NULL, " ");
        char* numberofflight = strtok(NULL, " ");
        char* points = strtok(NULL, "\n");

        // ���ָ������ݸ��Ƶ��ṹ����
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

        // ��ȡ�ѹ���ĺ�����Ϣ
        while (fgets(line, sizeof(line), file) && line[0] == ' ') {
            struct Flightlist* new_flight = (struct Flightlist*)malloc(sizeof(struct Flightlist));
            if (new_flight == NULL) {
                printf("�ڴ����ʧ��\n");
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

// �޸� login ����
LoginResult login() {
    LoginResult result = { NULL, NULL };
    struct cuslist* head = NULL;
    load_users(&head);

    char phone[12];
    char code[13];

    // ѭ����ȡ�û�������ֻ����룬ȷ������Ϊ11λ
    while (1) {
        printf("�������ֻ�����: ");
        scanf("%11s", phone);
        if (strlen(phone) != 11) {
            printf("�ֻ����볤��ӦΪ11λ�����������룡\n");
            continue;
        }
        break;
    }

    printf("����������: ");
    get_password(code, 13);

    struct cuslist* current = head;
    // �����û��б�����ƥ����ֻ����������
    while (current != NULL) {
        if (strcmp(current->data.phonum, phone) == 0 && strcmp(current->data.code, code) == 0) {
            result.current = current;
            result.head = head;

            // �����û��ѹ���ĺ�����Ϣ
            load_purchased_flights(&current->data);

            // �����¼�ɹ���ӭ��Ϣ
            printf("��¼�ɹ�����ӭ%s\n", current->data.name);

            return result;
        }
        current = current->next;
    }

    printf("�ֻ�������������\n");
    free_login_result(result);
    return result;
}

// �ͷ��û��б��ڴ�
void free_login_result(LoginResult result) {
    struct cuslist* temp;
    while (result.head != NULL) {
        temp = result.head;
        result.head = result.head->next;
        free(temp);
    }
}

// ��������Ϣ���浽�ļ�
void save_flights(struct Flightlist* head) {
    // ��ԭ�ļ�����д��
    FILE* file = fopen("flights.txt", "w");
    if (file == NULL) {
        perror("�޷��򿪺����ļ�����д��");
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

    // �򿪱����ļ�����д��
    FILE* backup_file = fopen("flights_backup.txt", "a");
    if (backup_file == NULL) {
        perror("�޷��򿪱����ļ�����д��");
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

// ��Ӻ���
void add_flight(struct Flightlist** head) {
    struct Flightlist* new_flight = (struct Flightlist*)malloc(sizeof(struct Flightlist));
    if (new_flight == NULL) {
        printf("�ڴ����ʧ�ܣ�\n");
        return;
    }

    printf("�����������: ");
    scanf("%s", new_flight->data.pdeparture);
    while (getchar() != '\n');  // ������뻺�����Ļ��з�

    printf("������Ŀ�ĵ�: ");
    scanf("%s", new_flight->data.pdestination);
    while (getchar() != '\n');  // ������뻺�����Ļ��з�

    printf("���������ʱ�䣨��ʽ�磺YYYY - MM - DD HH:MM:SS��: ");
    fgets(new_flight->data.tdaparture, sizeof(new_flight->data.tdaparture), stdin);
    // ȥ�� fgets ��ȡ�Ļ��з�
    size_t len = strlen(new_flight->data.tdaparture);
    if (len > 0 && new_flight->data.tdaparture[len - 1] == '\n') {
        new_flight->data.tdaparture[len - 1] = '\0';
    }

    printf("�����뵽��ʱ�䣨��ʽ�磺YYYY - MM - DD HH:MM:SS��: ");
    fgets(new_flight->data.tdestination, sizeof(new_flight->data.tdestination), stdin);
    // ȥ�� fgets ��ȡ�Ļ��з�
    len = strlen(new_flight->data.tdestination);
    if (len > 0 && new_flight->data.tdestination[len - 1] == '\n') {
        new_flight->data.tdestination[len - 1] = '\0';
    }

    printf("�����뺽����: ");
    scanf("%s", new_flight->data.number);
    while (getchar() != '\n');  // ������뻺�����Ļ��з�

    printf("����������λ��: ");
    scanf("%d", &new_flight->data.tnumber);
    while (getchar() != '\n');  // ������뻺�����Ļ��з�

    printf("��������λ����: ");
    scanf("%d", &new_flight->data.seat_rows);
    while (getchar() != '\n');  // ������뻺�����Ļ��з�

    printf("��������λ����: ");
    scanf("%d", &new_flight->data.seat_cols);
    while (getchar() != '\n');  // ������뻺�����Ļ��з�

    printf("������ͷ�Ȳ���λ��: ");
    scanf("%d", &new_flight->data.first_class_num);
    new_flight->data.first_class_remain = new_flight->data.first_class_num;
    while (getchar() != '\n');  // ������뻺�����Ļ��з�

    printf("������ͷ�Ȳ�Ʊ��: ");
    scanf("%f", &new_flight->data.first_class_price);
    while (getchar() != '\n');  // ������뻺�����Ļ��з�

    printf("�������������λ��: ");
    scanf("%d", &new_flight->data.business_class_num);
    new_flight->data.business_class_remain = new_flight->data.business_class_num;
    while (getchar() != '\n');  // ������뻺�����Ļ��з�

    printf("�����������Ʊ��: ");
    scanf("%f", &new_flight->data.business_class_price);
    while (getchar() != '\n');  // ������뻺�����Ļ��з�

    printf("�����뾭�ò���λ��: ");
    scanf("%d", &new_flight->data.economy_class_num);
    new_flight->data.economy_class_remain = new_flight->data.economy_class_num;
    while (getchar() != '\n');  // ������뻺�����Ļ��з�

    printf("�����뾭�ò�Ʊ��: ");
    scanf("%f", &new_flight->data.economy_class_price);
    while (getchar() != '\n');  // ������뻺�����Ļ��з�

    new_flight->next = *head;
    *head = new_flight;

    printf("������ӳɹ���\n");
    save_flights(*head); // ���溽����Ϣ���ļ�
}

// ɾ������
void delete_flight(struct Flightlist** head, const char* flight_number) {
    char departure[20], destination[20], departure_date[11];
    // ��ʾ�û�����Ҫɾ������ĳ�����
    printf("������Ҫɾ������ĳ�����: ");
    scanf("%19s", departure);
    // ��ʾ�û�����Ҫɾ�������Ŀ�ĵ�
    printf("������Ҫɾ�������Ŀ�ĵ�: ");
    scanf("%19s", destination);
    // ��ʾ�û�����Ҫɾ������ĳ�������
    printf("������Ҫɾ������ĳ������ڣ���ʽ��YYYY-MM-DD��: ");
    scanf("%10s", departure_date);

    struct Flightlist* current = *head;
    struct Flightlist* previous = NULL;

    // ���������б�����ƥ��ĺ���
    while (current != NULL &&
        (strcmp(current->data.number, flight_number) != 0 ||
            strcmp(current->data.pdeparture, departure) != 0 ||
            strcmp(current->data.pdestination, destination) != 0 ||
            strncmp(current->data.tdaparture, departure_date, 10) != 0)) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        // δ�ҵ�ƥ�亽�࣬��ʾ�û�
        printf("δ�ҵ��ú��ࡣ\n");
        return;
    }

    if (previous == NULL) {
        // ���Ҫɾ������ͷ�ڵ㣬����ͷָ��
        *head = current->next;
    }
    else {
        // ���򣬽�ǰһ���ڵ��nextָ��ָ��ǰ�ڵ����һ���ڵ�
        previous->next = current->next;
    }

    // �ͷŵ�ǰ�ڵ���ڴ�
    free(current);
    // ��ʾ�û�����ɾ���ɹ�
    printf("����ɾ���ɹ���\n");
    // ������º�ĺ�����Ϣ���ļ�
    save_flights(*head);
}

// �޸ĺ�����Ϣ
void modify_flight(struct Flightlist* head, const char* flight_number) {
    char departure[20], destination[20], departure_date[11];
    // ��ʾ�û�����Ҫ�޸ĺ���ĳ�����
    printf("������Ҫ�޸ĺ���ĳ�����: ");
    scanf("%19s", departure);
    // ��ʾ�û�����Ҫ�޸ĺ����Ŀ�ĵ�
    printf("������Ҫ�޸ĺ����Ŀ�ĵ�: ");
    scanf("%19s", destination);
    // ��ʾ�û�����Ҫ�޸ĺ���ĳ���������
    printf("������Ҫ�޸ĺ���ĳ��������գ���ʽ��YYYY-MM-DD��: ");
    scanf("%10s", departure_date);

    struct Flightlist* current = head;
    // ���������б�����ƥ��ĺ���
    while (current != NULL &&
        (strcmp(current->data.number, flight_number) != 0 ||
            strcmp(current->data.pdeparture, departure) != 0 ||
            strcmp(current->data.pdestination, destination) != 0 ||
            strncmp(current->data.tdaparture, departure_date, 10) != 0)) {
        current = current->next;
    }

    if (current == NULL) {
        // δ�ҵ�ƥ�亽�࣬��ʾ�û�
        printf("δ�ҵ��ú��࣡\n");
        return;
    }

    // ��ʾ�û������µĳ�����
    printf("�������µĳ�����: ");
    scanf("%s", current->data.pdeparture);
    // ��ʾ�û������µ�Ŀ�ĵ�
    printf("�������µ�Ŀ�ĵ�: ");
    scanf("%s", current->data.pdestination);

    // ��ʾ�û������µĳ���ʱ��
    printf("�������µĳ���ʱ�䣨��ʽ�磺YYYY-MM-DD HH:MM:SS��: ");
    scanf("%s", current->data.tdaparture);
    // ����������еĻ��з�
    while (getchar() != '\n');

    // ��ʾ�û������µ����ʱ��
    printf("�������µ����ʱ�䣨��ʽ�磺YYYY-MM-DD HH:MM:SS��: ");
    scanf("%s", current->data.tdestination);
    // ����������еĻ��з�
    while (getchar() != '\n');

    // ��ʾ�û������µ�����λ��
    printf("�������µ�����λ��: ");
    scanf("%d", &current->data.tnumber);
    // ��ʾ�û������µ���λ����
    printf("�������µ���λ����: ");
    scanf("%d", &current->data.seat_rows);
    // ��ʾ�û������µ���λ����
    printf("�������µ���λ����: ");
    scanf("%d", &current->data.seat_cols);
    // ��ʾ�û������µ�ͷ�Ȳ���λ��
    printf("�������µ�ͷ�Ȳ���λ��: ");
    scanf("%d", &current->data.first_class_num);
    current->data.first_class_remain = current->data.first_class_num;
    // ��ʾ�û������µ�ͷ�Ȳռ۸�
    printf("�������µ�ͷ�Ȳռ۸�: ");
    scanf("%f", &current->data.first_class_price);
    // ��ʾ�û������µ��������λ��
    printf("�������µ��������λ��: ");
    scanf("%d", &current->data.business_class_num);
    current->data.business_class_remain = current->data.business_class_num;
    // ��ʾ�û������µ�����ռ۸�
    printf("�������µ�����ռ۸�: ");
    scanf("%f", &current->data.business_class_price);
    // ��ʾ�û������µľ��ò���λ��
    printf("�������µľ��ò���λ��: ");
    scanf("%d", &current->data.economy_class_num);
    current->data.economy_class_remain = current->data.economy_class_num;
    // ��ʾ�û������µľ��òռ۸�
    printf("�������µľ��òռ۸�: ");
    scanf("%f", &current->data.economy_class_price);

    // ��ʾ�û�������Ϣ�޸ĳɹ�
    printf("������Ϣ�޸ĳɹ���\n");
    // ���溽����Ϣ���ļ�
    save_flights(head);
}

// ��ѯ������Ϣ
void query_flight(struct Flightlist* head) {
    char departure[20], destination[20];
    char start_date[11], end_date[11];
    int max_days = 7; // �������ڷ�Χ���������

    // ��ʾ�û����������
    printf("�����������: ");
    scanf("%19s", departure);
    // ��ʾ�û�����Ŀ�ĵ�
    printf("������Ŀ�ĵ�: ");
    scanf("%19s", destination);
    // ��ʾ�û�����������ڷ�Χ
    printf("������������ڷ�Χ����ʽ: YYYY-MM-DD YYYY-MM-DD�����Χ %d ��  ʾ����2025-01-01 2025-01-07��: ", max_days);
    scanf("%10s %10s", start_date, end_date);

    // ������ڷ�Χ�Ƿ񳬹��������
    time_t time_start = str_to_time(start_date);
    time_t time_end = str_to_time(end_date);
    if (time_start == -1 || time_end == -1) {
        // ���ڽ���ʧ�ܣ���ʾ�û�������ȷ�����ڸ�ʽ
        printf("���ڽ���ʧ�ܣ���������ȷ�����ڸ�ʽ��\n");
        return;
    }
    double diff = difftime(time_end, time_start) / (60 * 60 * 24);
    if (diff > max_days) {
        // ���ڷ�Χ���������������ʾ�û���������
        printf("���ڷ�Χ���� %d �죬���������롣\n", max_days);
        return;
    }

    struct Flightlist* current = head;
    int found = 0;
    // ���������б����ҷ��������ĺ���
    while (current != NULL) {
        // ��ȡ��������
        char flight_date[11];
        strncpy(flight_date, current->data.tdaparture, 10);
        flight_date[10] = '\0';

        time_t time_flight = str_to_time(flight_date);
        if (time_flight == -1) {
            // �������ڽ���ʧ�ܵĺ���
            current = current->next;
            continue;
        }

        // ����Ƿ��������
        if (strcmp(current->data.pdeparture, departure) == 0 &&
            strcmp(current->data.pdestination, destination) == 0 &&
            time_flight >= time_start && time_flight <= time_end) {
            // ������������ĺ�����Ϣ
            printf("������: %s\n", current->data.pdeparture);
            printf("Ŀ�ĵ�: %s\n", current->data.pdestination);
            printf("����ʱ��: %s\n", current->data.tdaparture);
            printf("���ʱ��: %s\n", current->data.tdestination);
            printf("������: %s\n", current->data.number);
            printf("����λ��: %d\n", current->data.tnumber);
            printf("��λ����: %d\n", current->data.seat_rows);
            printf("��λ����: %d\n", current->data.seat_cols);
            printf("ͷ�Ȳ���λ��: %d, ʣ����λ��: %d, �۸�: %.2f\n", current->data.first_class_num, current->data.first_class_remain, current->data.first_class_price);
            printf("�������λ��: %d, ʣ����λ��: %d, �۸�: %.2f\n", current->data.business_class_num, current->data.business_class_remain, current->data.business_class_price);
            printf("���ò���λ��: %d, ʣ����λ��: %d, �۸�: %.2f\n", current->data.economy_class_num, current->data.economy_class_remain, current->data.economy_class_price);
            printf("------------------------\n");
            found = 1;
        }
        current = current->next;
    }
    if (!found) {
        // δ�ҵ����������ĺ��࣬��ʾ�û�
        printf("δ�ҵ����������ĺ��࣡\n");
    }
}

// �ͷ������ڴ�
void free_flight_list(struct Flightlist* head) {
    struct Flightlist* temp;
    // ���������ͷ�ÿ���ڵ���ڴ�
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

// ���в�������
void flight_operation() {
    // ֱ��ʹ��ȫ�ֱ���
    struct Flightlist* flightHead = global_flight_head;
    // ���غ�����Ϣ
    load_flights(&flightHead);

    while (1) {
        // ��ʾ�����˵�
        printf("1. ��Ӻ���\n");
        printf("2. ɾ������\n");
        printf("3. �޸ĺ�����Ϣ\n");
        printf("4. ��ѯ������Ϣ\n");
        printf("0. �˳�\n");
        int choice;
        scanf("%d", &choice);
        char flightNumber[10];

        switch (choice) {
        case 1:
            // ������Ӻ��ຯ��
            add_flight(&flightHead);
            break;
        case 2:
            // ��ʾ�û�����Ҫɾ���ĺ����
            printf("������Ҫɾ���ĺ����: ");
            scanf("%s", flightNumber);
            // ����ɾ�����ຯ��
            delete_flight(&flightHead, flightNumber);
            break;
        case 3:
            // ��ʾ�û�����Ҫ�޸ĵĺ����
            printf("������Ҫ�޸ĵĺ����: ");
            scanf("%s", flightNumber);
            // �����޸ĺ�����Ϣ����
            modify_flight(flightHead, flightNumber);
            break;
        case 4:
            // ���ø߼���ѯ����
            query_flight(flightHead);
            break;
        case 0:
            // �ͷ������ڴ�
            free_flight_list(flightHead);
            return;
        default:
            // ��Ч��ѡ����ʾ�û���������
            printf("��Ч��ѡ�����������룡\n");
        }
    }
}

// ���� load_all_customers ���������ڼ��������û���Ϣ
struct cuslist* load_all_customers() {
    struct cuslist* head = NULL;
    // ���� load_users ���������û���Ϣ
    load_users(&head);
    return head;
}

// ��ȡ��Ʊ��Ϣ�����浽�ļ���ͬʱ�����û��ѹ������б�
void read_purchase_info(struct Flightlist* newPurchased, struct cusman* customer, int seatClass) {
    char name[20];
    char id[20];

    printf("������˿�����: ");
    fgets(name, sizeof(name), stdin);
    size_t len = strlen(name);
    if (len > 0 && name[len - 1] == '\n') {
        name[len - 1] = '\0';
    }
    strcpy(newPurchased->passenger_name, name);

    printf("������˿����֤��: ");
    fgets(id, sizeof(id), stdin);
    len = strlen(id);
    if (len > 0 && id[len - 1] == '\n') {
        id[len - 1] = '\0';
    }
    strcpy(newPurchased->passenger_id, id);

    // ��ӡ��Ʊ��ʾ��Ϣ
    printf("���ڳ�Ʊ�У������ĵȺ�\n");

    FILE* fp = fopen("ticket_info.txt", "a");
    if (fp == NULL) {
        perror("�޷����ļ�");
        return;
    }

    const char* seat_class_name;
    switch (seatClass) {
    case 1:
        seat_class_name = "ͷ�Ȳ�";
        break;
    case 2:
        seat_class_name = "�����";
        break;
    case 3:
        seat_class_name = "���ò�";
        break;
    default:
        seat_class_name = "δ֪��λ";
    }

    // ����Ʊ��Ϣд���ļ�
    fprintf(fp, "�˿�����:%s\n", newPurchased->passenger_name);
    fprintf(fp, "���֤��:%s\n", newPurchased->passenger_id);
    fprintf(fp, "�����:%s\n", newPurchased->data.number);
    fprintf(fp, "����ʱ��:%s\n", newPurchased->data.tdaparture);
    fprintf(fp, "��λ����:%s\n", seat_class_name);
    fprintf(fp, "�û��ֻ���:%s\n\n", customer->phonum);

    fclose(fp);

    // �����û��� purchased_flights �б�
    newPurchased->next = customer->purchased_flights;
    customer->purchased_flights = newPurchased;
    customer->numberofflight++;
    save_customers(load_all_customers()); // ���������û���Ϣ
}

// ���򺽰�Ʊ�ĺ���
void buy(struct Flightlist* head, struct cusman* customer) {
    char flightNumber[10];
    char departure[20];
    char destination[20];
    char departureDate[11];
    int seatClass;

    printf("��������Ҫ����ĺ�����:");
    scanf("%s", flightNumber);
    printf("�����������:");
    scanf("%s", departure);
    printf("������Ŀ�ĵ�:");
    scanf("%s", destination);
    printf("���������ʱ��(��ʽ��YYYY-MM-DD):");
    scanf("%10s", departureDate);

    struct Flightlist* current = head;
    // ����ƥ��ĺ���
    while (current != NULL && (strcmp(current->data.number, flightNumber) != 0 ||
        strcmp(current->data.pdeparture, departure) != 0 ||
        strcmp(current->data.pdestination, destination) != 0 ||
        strncmp(current->data.tdaparture, departureDate, 10) != 0)) {
        current = current->next;
    }

    if (current == NULL) {
        printf("δ�ҵ��ú��ࡣ\n");
        return;
    }

    printf("��ѡ����λ���ͣ�1. ͷ�Ȳ� 2. ����� 3. ���ò�: ");
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
            printf("ͷ�Ȳ�����ʣ����λ��\n");
            return;
        }
        break;
    case 2:
        if (current->data.business_class_remain > 0) {
            current->data.business_class_remain--;
            price = current->data.business_class_price;
        }
        else {
            printf("���������ʣ����λ��\n");
            return;
        }
        break;
    case 3:
        if (current->data.economy_class_remain > 0) {
            current->data.economy_class_remain--;
            price = current->data.economy_class_price;
        }
        else {
            printf("���ò�����ʣ����λ��\n");
            return;
        }
        break;
    default:
        printf("��Ч����λ����ѡ��\n");
        return;
    }

    printf("��Ʊ%s�ɹ���Ʊ��Ϊ%.2f\n", seatClass == 1 ? "ͷ�Ȳ�" : (seatClass == 2 ? "�����" : "���ò�"), price);
    struct Flightlist* newPurchased = (struct Flightlist*)malloc(sizeof(struct Flightlist));
    newPurchased->data = current->data;
    read_purchase_info(newPurchased, customer, seatClass);

    // ȷ���½ڵ���ȷ��ӵ�������
    newPurchased->next = customer->purchased_flights;
    customer->purchased_flights = newPurchased;
    save_flights(head);
    printf("��Ʊ�ɹ���\n");
}

// ��ѯ�û���Ԥ���ĺ�����Ϣ
void query_purchased_flights(struct cusman* customer) {
    struct Flightlist* current = customer->purchased_flights;
    if (current == NULL) {
        printf("��û��Ԥ���κκ��ࡣ\n");
        return;
    }

    // ���ڼ�¼�Ѿ����ʹ��Ľڵ㣬����ѭ������
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

        printf("������: %s\n", current->data.pdeparture);
        printf("Ŀ�ĵ�: %s\n", current->data.pdestination);
        printf("����ʱ��: %s\n", current->data.tdaparture);
        printf("����ʱ��: %s\n", current->data.tdestination);
        printf("������: %s\n", current->data.number);
        printf("------------------------\n");
        current = current->next;
    }

    free(visited);
}

// �����û��ѹ���ĺ�����Ϣ�������ظ�����
void load_purchased_flights(struct cusman* customer) {
    if (customer->purchased_flights != NULL) {
        return; // ����Ѿ����ع���ֱ�ӷ���
    }
    FILE* fp = fopen("ticket_info.txt", "r");
    if (fp == NULL) {
        return;
    }

    char line[100];
    char name[20], id[20], flight_number[10], departure_time[20], seat_class[20], phone[12];

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (sscanf(line, "�˿�����:%19s", name) == 1) {
            fgets(line, sizeof(line), fp);
            sscanf(line, "���֤��:%19s", id);
            fgets(line, sizeof(line), fp);
            sscanf(line, "������:%9s", flight_number);
            fgets(line, sizeof(line), fp);
            sscanf(line, "����ʱ��:%19s", departure_time);
            fgets(line, sizeof(line), fp);
            sscanf(line, "��λ����:%19s", seat_class);
            fgets(line, sizeof(line), fp);
            sscanf(line, "�û��ֻ���:%11s", phone);

            if (strcmp(phone, customer->phonum) == 0) {
                struct Flightlist* new_flight = (struct Flightlist*)malloc(sizeof(struct Flightlist));
                if (new_flight == NULL) {
                    printf("�ڴ����ʧ�ܣ�\n");
                    continue;
                }

                // ���ݺ����Ų��Һ�����ϸ��Ϣ
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

// �޸��û�����
void modify_password(struct cuslist* current, struct cuslist* head) {
    char password2[15];
    int passwordmatch = 0;
    if (current == NULL) {
        printf("���ȵ�¼��\n");
        return;
    }

    char old_password[13];
    char new_password[13];

    printf("�����������: ");
    get_password(old_password, sizeof(old_password));

    if (strcmp(current->data.code, old_password) != 0) {
        printf("�������������\n");
        return;
    }

    while (passwordmatch == 0) {
        printf("������������: \n");
        get_password(new_password, sizeof(new_password));

        printf("���ٴ�����: \n");
        get_password(password2, sizeof(password2));

        if (strcmp(new_password, password2) == 0) {
            strcpy(current->data.code, new_password);
            passwordmatch = 1;
            printf("�����޸ĳɹ���\n");
        }
        else {
            printf("������������벻һ�£����������룡\n");
        }
    }

    strcpy(current->data.code, new_password);

    printf("�����޸ĳɹ���\n");
}

// ������뻺��������������ַ�Ӱ���������
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// ��ʾ�û��ķ��м�¼
void flightRecord(struct cusman* customer) {
    // �򵥼�����м�¼�����ѹ���ĺ����¼
    query_purchased_flights(customer);
}

// �Զ���Ĳ����ִ�Сд���ַ������Һ��������� 1 ��ʾƥ�䣬0 ��ʾ��ƥ��
int strcasestr(const char* haystack, const char* needle) {
    if (!*needle) return 1; // ��� needle Ϊ�գ�����Ϊƥ��
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

// ������뺽�������ʣ��ʱ��
long Calculate_time(const char* departure_time) {
    struct tm tm = { 0 };
    // ����ʱ���ַ���
    if (strptime(departure_time, "%Y-%m-%d %H:%M:%S", &tm) == NULL) {
        // �������ʧ�ܣ�����ֻ�������ڲ���
        if (strptime(departure_time, "%Y-%m-%d", &tm) == NULL) {
            return -1; // ����ʧ�ܣ����� -1
        }
    }
    time_t flight_time = mktime(&tm);
    time_t current_time = time(NULL);
    return flight_time - current_time;
}

// ��ʾ���뺽�������ʣ��ʱ�䣬��������������
void Display_remainingtime(long seconds) {
    int security_checktime = 2 * 3600;
    if (seconds < 0) {
        printf("�ú��������\n");
    }
    else {
        int days = seconds / (24 * 3600);
        seconds %= (24 * 3600);
        int hours = seconds / 3600;
        seconds %= 3600;
        int minutes = seconds / 60;
        seconds %= 60;

        if (days > 0) {
            printf("���뺽����ɻ���%d��%dСʱ%d����%d��\n", days, hours, minutes, seconds);
        }
        else if (hours > 0) {
            printf("���뺽����ɻ���%dСʱ%d����%d��\n", hours, minutes, seconds);
        }
        else if (minutes > 0) {
            printf("���뺽����ɻ���%d����%d��\n", minutes, seconds);
        }
        else {
            printf("���뺽����ɻ���%d��\n", seconds);
        }

        // ��������
        if ((hours * 3600 + minutes * 60 + seconds) < security_checktime) {
            printf("��ܰ��ʾ�����뺽����ɲ���2Сʱ���뾡��ǰ�����죡\n");
        }
    }
}

// �û�ģ����ѯ�Լ��ĺ���Ԥ����Ϣ
void fuzzy_search_bookings(struct cusman* current_user) {
    if (current_user == NULL) {
        printf("��ǰ�û�δ��¼��\n");
        return;
    }

    char input[200];
    // ������뻺����
    while (getchar() != '\n');
    printf("������ؼ��ʣ��ؼ���֮���ÿո�ָ����������ʮ���ؼ���: ");
    fgets(input, sizeof(input), stdin);
    // �����ַ���ĩβ�Ļ��з�
    input[strcspn(input, "\n")] = 0;

    // �ָ�ؼ���
    char* keywords[10];
    int keyword_count = 0;

    char* token = strtok(input, " "); // �ָ�ؼ��ʵ�keywords������
    while (token != NULL && keyword_count < 10) {
        keywords[keyword_count++] = token;
        token = strtok(NULL, " ");
    }

    if (keyword_count == 0) {
        printf("����������һ���ؼ��ʡ�\n");
        return;
    }

    struct Flightlist* flight = current_user->purchased_flights;
    // ���ڱ���Ѿ����ʹ��Ľڵ�
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

        int all_keywords_match = 1; // ���ڼ�¼�Ƿ�ƥ�����йؼ���
        for (int i = 0; i < keyword_count; i++) {
            if (!strcasestr(flight->data.number, keywords[i]) && !strcasestr(flight->data.pdeparture, keywords[i]) && !strcasestr(flight->data.pdestination, keywords[i])) {
                all_keywords_match = 0;
                break;
            }
        }

        if (all_keywords_match) {
            printf("\n�˿�����:%s\n", flight->passenger_name);
            printf("֤������:%s\n", flight->passenger_id);
            printf("�����:%s\n", flight->data.number);
            printf("����:%s �� %s\n", flight->data.pdeparture, flight->data.pdestination);
            printf("����ʱ��:%s\n", flight->data.tdaparture);
            long remaining_seconds = Calculate_time(&flight->data.tdaparture); // ȷ�� Calculate_time ��ȷʵ��
            Display_remainingtime(remaining_seconds); // ��ʾʣ��ʱ��
            found = 1;
        }
        flight = flight->next;
    }

    free(visited);

    if (!found) {
        printf("δ�ҵ�ƥ��ĺ����¼��\n");
    }
}

// �޸Ķ�����Ϣ
int reflightinformation(struct cuslist* current, struct cuslist* head) {
    if (current == NULL) {
        printf("����: �û���ϢΪ��!\n");
        return 0;
    }
    int count = 0;
    // ��ʾ��Ԥ���ĺ�����Ϣ
    struct Flightlist* temp = current->data.purchased_flights;
    // ���ڱ���Ѿ����ʹ��Ľڵ�
    struct Flightlist** visited = (struct Flightlist**)malloc(1000 * sizeof(struct Flightlist*));
    int visited_count = 0;

    if (temp == NULL) {
        free(visited);
        printf("����û��Ԥ���κκ��ࡣ\n");
        return 0;
    }
    // ͳ����Ԥ���ĺ�������
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

        printf("%d. ������: %s -> Ŀ�ĵ�: %s, ����ʱ��: %s, �����: %s\n", count + 1, temp->data.pdeparture, temp->data.pdestination, temp->data.tdaparture, temp->data.number);
        count++;
        temp = temp->next;
    }

    int choice;
    printf("��ѡ��Ҫ�޸ĵĺ������(1-%d)\n", count);
    // ��ȡ�û�ѡ��
    if (scanf("%d", &choice) != 1) {
        printf("������Ч������������: ");
        free(visited);
        return 0;
    }
    // ������뻺�����Ļ��з�
    getchar();
    if (choice < 1 || choice > count) {
        printf("��Чѡ��\n");
        free(visited);
        return 0;
    }
    struct Flightlist* selected_flight = current->data.purchased_flights;
    // �ҵ��û�ѡ��ĺ���
    for (int i = 1; i < choice; i++) {
        selected_flight = selected_flight->next;
    }
    int modify_choice;
    while (1) {
        printf("��ѡ��Ҫ�޸ĵ���Ŀ:\n");
        printf("1. �޸ĳ˿�����\n");
        printf("2. �޸�֤������\n");
        printf("3. ��Ʊ\n");
        printf("4. ����\n");
        printf("����������ѡ�� (1-4): ");
        // ��ȡ�û�ѡ��
        if (scanf("%d", &modify_choice) != 1) {
            printf("������Ч������������: ");
            // ������뻺����
            while (getchar() != '\n');
            continue;
        }
        // ������뻺�����Ļ��з�
        getchar();
        if (modify_choice == 4) {
            break;
        }
        switch (modify_choice) {
        case 1:
            printf("�������µĳ˿����� (��ǰ: %s): ", selected_flight->passenger_name);
            fgets(selected_flight->passenger_name, sizeof(selected_flight->passenger_name), stdin);
            selected_flight->passenger_name[strcspn(selected_flight->passenger_name, "\n")] = '\0';
            printf("�����Ѹ���Ϊ: %s\n", selected_flight->passenger_name);
            break;
        case 2:
            printf("�������µ�֤������ (��ǰ: %s): ", selected_flight->passenger_id);
            fgets(selected_flight->passenger_id, sizeof(selected_flight->passenger_id), stdin);
            selected_flight->passenger_id[strcspn(selected_flight->passenger_id, "\n")] = '\0';
            printf("֤�������Ѹ���Ϊ: %s\n", selected_flight->passenger_id);
            break;
            // �޸ĺ����Ʊ�߼�
        case 3:
            // ��Ʊ�߼����޸���λ״̬Ϊδռ�ã���Ԥ�����б����Ƴ��ú���
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

                // ɾ�� ticket_info.txt �еĶ�Ӧ��Ϣ
                FILE* fp = fopen("ticket_info.txt", "r");
                if (fp == NULL) {
                    perror("�޷����ļ�");
                }
                else {
                    FILE* temp_fp = fopen("temp.txt", "w");
                    if (temp_fp == NULL) {
                        perror("�޷�������ʱ�ļ�");
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
                                if (strstr(line, "�˿�����:") != NULL) {
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
                current_flight = NULL; // ��������ָ��
                selected_flight = NULL; // ��������ָ��
                printf("��Ʊ�ɹ�\n");
                free(visited);
                return 0; // ֱ���˳��˵�
            }
            break;
        }
    }
    free(visited);
    return 1;
}

// �޸��û���Ϣ�ĺ���
void reviseinformation(struct cuslist* current_user, struct cuslist* head) {
    if (current_user == NULL) {
        printf("��ǰ�û���ϢΪ��\n");
        return;
    }

    while (1) {
        // ��ʾ��ǰ�û���Ϣ
        printf("\n=== ��ǰ�û���Ϣ ===\n");
        printf("1. ����: %s\n", current_user->data.name);
        printf("2. �ֻ�����: %s\n", current_user->data.phonum);
        printf("3. ���֤��: %s\n", current_user->data.idnumber);
        printf("4. �Ա�: %s\n", current_user->data.sex);
        printf("5. ����:\n");
        printf("6. ��ע��Ϣ: %s\n", current_user->data.note);
        printf("7. �˳��޸�ϵͳ\n");

        printf("\n������Ҫ�޸ĵ���Ϣ����(1-7): ");
        int choice;
        if (scanf("%d", &choice) != 1) {
            printf("������Ч��������һ������\n");
            while (getchar() != '\n'); // ������뻺����
            continue;
        }

        if (choice == 7) {
            break;
        }

        if (choice < 1 || choice > 7) {
            printf("������Ч��������1-7֮�������\n");
            continue;
        }

        switch (choice) {
        case 1:
            printf("�������µ�����: ");
            scanf("%s", current_user->data.name);
            break;

        case 2:
            while (1) {
                printf("�������µ��ֻ�����(11λ): ");
                scanf("%11s", current_user->data.phonum);
                if (strlen(current_user->data.phonum) != 11) {
                    printf("�ֻ�����ӦΪ11λ������������\n");
                    continue;
                }
                break;
            }
            break;

        case 3:
            while (1) {
                printf("�������µ����֤��: ");
                scanf("%s", current_user->data.idnumber);
                if (strlen(current_user->data.idnumber) != 18) {
                    printf("���֤��ӦΪ18λ������������\n");
                    continue;
                }
                break;
            }
            break;

        case 4:
            printf("�������µ��Ա�: ");
            scanf("%s", current_user->data.sex);
            break;

        case 5:
            modify_password(current_user, head);
            break;

        case 6:
            printf("�������µı�ע��Ϣ: ");
            scanf("%s", current_user->data.note);
            break;
        }

        // �����޸ĺ���û���Ϣ���ļ�
        FILE* file = fopen("customers.txt", "w");
        if (file == NULL) {
            printf("�޷����ļ�\n");
            return;
        }
        // �������û�����Ϣд���ļ�
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

        while (getchar() != '\n'); // ������뻺����
    }

    printf("��Ϣ�޸����\n");
    save_customers(head);
}

// �û������˵�����
void user_operation(struct cuslist* current, struct cuslist* head) {
    // ֱ��ʹ��ȫ�ֱ���
    struct Flightlist* flightHead = global_flight_head;

    while (1) {
        // ��ʾ�����˵�ѡ��
        printf("1. �޸��û���Ϣ\n");
        printf("2. �޸ĵǻ���Ϣ\n");
        printf("3. ��ѯ������Ϣ\n");
        printf("4. �����Ʊ\n");
        printf("5. ģ����ѯԤ����¼\n");
        printf("6. �鿴��Ʊ��¼\n");
        printf("0. �˳���¼\n");
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
            printf("��Ч��ѡ�����������룡\n");
        }
    }
}

// ��������Ƿ����ѭ���ĺ���
int has_cycle(struct Flightlist* head) {
    struct Flightlist* slow = head;
    struct Flightlist* fast = head;

    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;

        if (slow == fast) {
            return 1; // ����ѭ��
        }
    }
    return 0; // ������ѭ��
}

// �ͷ��û���Ϣ�����ڴ�ĺ���
void free_cuslist(struct cuslist* head) {
    struct cuslist* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}
