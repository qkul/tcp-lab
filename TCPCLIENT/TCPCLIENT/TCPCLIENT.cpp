﻿#include "pch.h"

#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")

#include <winsock2.h>
#include <windows.h>
#include <iostream>

using namespace std;

#define PORT 666
#define SERVERADDR "127.0.0.1"

int main(int argc, char* argv[])
{
	char buff[1024];
	printf("TCP DEMO CLIENT\n");

	// Шаг 1 - инициализация библиотеки Winsock
	if (WSAStartup(0x202, (WSADATA *)&buff[0]))
	{
		printf("WSAStart error %d\n", WSAGetLastError());
		return -1;
	}

	// Шаг 2 - создание сокета
	SOCKET my_sock;
	my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (my_sock < 0)
	{
		printf("Socket() error %d\n", WSAGetLastError());
		return -1;
	}

	// Шаг 3 - установка соединения

	// заполнение структуры sockaddr_in
	// указание адреса и порта сервера
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);
	HOSTENT *hst;

	// преобразование IP адреса из символьного в
	// сетевой формат
	if (inet_addr(SERVERADDR) != INADDR_NONE)
		dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
	else
		// попытка получить IP адрес по доменному
		// имени сервера
		if (hst = gethostbyname(SERVERADDR))
			// hst->h_addr_list содержит не массив адресов,
			// а массив указателей на адреса
			((unsigned long *)&dest_addr.sin_addr)[0] =
			((unsigned long **)hst->h_addr_list)[0][0];
		else
		{
			printf("Invalid address %s\n", SERVERADDR);
			closesocket(my_sock);
			WSACleanup();
			return -1;
		}

	// адрес сервера получен – пытаемся установить
	// соединение 
	if (connect(my_sock, (sockaddr *)&dest_addr,
		sizeof(dest_addr)))
	{
		printf("Connect error %d\n", WSAGetLastError());
		return -1;
	}

	printf("Соединение с %s успешно установлено\n\
    Type quit for quit\n\n", SERVERADDR);

	// Шаг 4 - чтение и передача сообщений
	int nsize;
	while ((nsize = recv(my_sock, &buff[0],
		sizeof(buff) - 1, 0))
		!= SOCKET_ERROR)
	{
		// ставим завершающий ноль в конце строки 
		buff[100] = 0;

		// выводим на экран 
		printf("S=>C:%s", buff);

		// читаем пользовательский ввод с клавиатуры
		printf("S<=C:"); 
		//fgets(&buff[0], sizeof(buff) - 1, stdin);
		string str = "";
		char ch;
		do
		{
			ch = _getche();
			if ((int)ch != 3281) {
				str += ch;
			}
			else {
				break;
			}

		} while (true);

		strcpy(buff, str.c_str());

		// проверка на "quit"
		if (!strcmp(&buff[0], "quit\n"))
		{
			// Корректный выход
			printf("Exit...");
			closesocket(my_sock);
			WSACleanup();
			return 0;
		}

		// передаем строку клиента серверу
		send(my_sock, &buff[0], nsize, 0);
	}

	printf("Recv error %d\n", WSAGetLastError());
	closesocket(my_sock);
	WSACleanup();
	return -1;
}
