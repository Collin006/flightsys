#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h> // ����ʹ�� _access ��������ļ��Ƿ����
#include "structure.h"
#include "flight_generator.h"

// ���� generate_yearly_flight_info ����
extern void generate_yearly_flight_info();

// ȫ�ֱ��������ڴ洢������Ϣ�����ͷָ��
extern struct Flightlist* global_flight_head = NULL;

int main() {
    struct Flightlist* flightHead = NULL;

    // ��ӡ��ӭ��ʾ��Ϣ
    printf("����Ʊ��Ԥ��ϵͳ��������...\n");

    // ��� flights.txt �ļ��Ƿ����
    if (_access("flights.txt", 0) != 0) {
        // �ļ������ڣ����ɺ�����Ϣ
        generate_yearly_flight_info();
    }

    // ���غ�����Ϣ
    load_flights(&flightHead);

    while (1) {
        printf("��ӭʹ�ú���Ʊ��Ԥ��ϵͳ��\n");
        printf("1. �����û�\n");
        printf("2. ���ǹ���Ա\n");
        printf("0. �˳�\n");
        int choice;
        scanf("%d", &choice);
        switch (choice)
        {
        case 1: // ��ͨ�û�
            while (1) {
                printf("1. ע��\n");
                printf("2. ��¼\n");
                printf("0. ����\n");
                int choice1;
                scanf("%d", &choice1);
                switch (choice1)
                {
                case 1: // ע��
                    register_customer();
                    break;
                case 2: // ��¼
                {
                    LoginResult loginResult = login();
                    if (loginResult.current != NULL) {
                        user_operation(loginResult.current, loginResult.head);
                        while (1) {
                            printf("1. ��������\n");
                            printf("0. �˳���¼\n");
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
                                printf("��Чѡ�����������롣\n");
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
        case 2: // ϵͳ����Ա
        {
            char input[7];
            int i = 0;
            char ch;
            printf("�����뵥λ����Ա��֤��: ");
            while (1) {
                ch = getch();  // ��ȡ�ַ�������ʾ
                if (ch == '\r') {
                    input[i] = '\0';  // �ַ�������
                    printf("\n");
                    break;
                }
                // ������˸����ɾ��ǰһ���ַ�
                else if (ch == '\b' && i > 0) {
                    i--;
                    printf("\b \b");
                }
                // ����ǿɴ�ӡ�ַ���δ�ﵽ��󳤶�
                else if (i < 6 && (isprint(ch) || isspace(ch))) {
                    input[i++] = ch;
                    printf("*");
                }
            }
            if (strcmp(input, "FJHBCC") == 0) {
                // ���� flight_operation �������к���������
                flight_operation();
            }
            else {
                printf("��֤���������\n");
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