#pragma once
#define CURL_STATICLIB
#include "Auth\Curl\curl.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <atlsecurity.h>
#include <wincrypt.h>
#include <iphlpapi.h>
#include <wbemidl.h>
#include <comdef.h>
#include "json.hpp"
#include <thread>
#include <mutex>
#include <atomic>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <vector>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Normaliz.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Wldap32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "wbemuuid.lib")

struct ApiResponse {
    bool success = false;
    std::string message = "";
    std::string username = "";
    std::string productName = "";
    int daysRemaining = 0;
    std::string expirationDate = "";
    int timesUsed = 0;
    int maxUsers = 0;
    bool isBanned = false;
    std::string rawResponse = "";
};

class NashApi {
private:
    using json = nlohmann::json;

    std::string apiUrl;
    std::mutex responseMutex;
    bool curlInitialized = false;

    ApiResponse currentResponse;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

    bool InitializeCurl();
    void CleanupCurl();
    std::string PerformRequest(const std::string& endpoint, const std::string& postData = "");

    std::string GetIPAddress();
    std::string GetHWID();
    std::string GetMotherboardID();
    std::string GetGPUId();
    std::string GetRAMId();
    std::string GetFallbackHWID();
    std::string GetVolumeId();
    std::string toLower(const std::string& str);
    void ParseResponse(const std::string& response, const std::string& operation);

public:
    NashApi(const std::string& baseUrl = "https://nashapi.squareweb.app/");
    ~NashApi();

    ApiResponse GetApiResponse();
    void SetApiResponse(const ApiResponse& response);

    bool Setup();
    bool PerformLogin(const std::string& username, const std::string& password);
    bool PerformRegister(const std::string& username, const std::string& password, const std::string& license);
    bool PerformLoginWithLicense(const std::string& license);
};

extern NashApi g_NashApi;
bool InitializeNashApi();