#include "api.hpp"
#include <vector>

NashApi g_NashApi;

size_t NashApi::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    ((std::string*)userp)->append((char*)contents, totalSize);
    return totalSize;
}

bool NashApi::InitializeCurl() {
    if (curlInitialized) return true;

    CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (res == CURLE_OK) {
        curlInitialized = true;
        return true;
    }
    return false;
}

void NashApi::CleanupCurl() {
    if (curlInitialized) {
        curl_global_cleanup();
        curlInitialized = false;
    }
}

std::string NashApi::PerformRequest(const std::string& endpoint, const std::string& postData) {
    if (!InitializeCurl()) {
        return "{\"success\":false,\"message\":\"Failed to initialize CURL\"}";
    }

    std::string response;
    CURL* curl = curl_easy_init();

    if (!curl) {
        return "{\"success\":false,\"message\":\"Failed to initialize CURL handle\"}";
    }

    std::string url = apiUrl + endpoint;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Nash API Client/1.0");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

    if (!postData.empty()) {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_slist_free_all(headers);
    }

    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        return "{\"success\":false,\"message\":\"CURL error: " + std::string(curl_easy_strerror(res)) + "\"}";
    }

    if (http_code != 200) {
        return "{\"success\":false,\"message\":\"HTTP error: " + std::to_string(http_code) + "\"}";
    }

    return response.empty() ? "{\"success\":false,\"message\":\"Empty response\"}" : response;
}

std::string NashApi::GetIPAddress() {
    if (!InitializeCurl()) return "127.0.0.1";

    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.ipify.org?format=json");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    try {
        if (!response.empty()) {
            json j = json::parse(response);
            return j.value("ip", "127.0.0.1");
        }
    }
    catch (...) {
    }

    return "127.0.0.1";
}

std::string NashApi::GetHWID() {
    std::vector<std::string> hardwareComponents;

    std::string mbId = GetMotherboardID();
    if (!mbId.empty()) {
        hardwareComponents.push_back("MB:" + mbId);
    }

    std::string gpuId = GetGPUId();
    if (!gpuId.empty()) {
        hardwareComponents.push_back("GPU:" + gpuId);
    }

    std::string ramId = GetRAMId();
    if (!ramId.empty()) {
        hardwareComponents.push_back("RAM:" + ramId);
    }

    if (hardwareComponents.empty()) {
        return GetFallbackHWID();
    }

    std::string result;
    for (size_t i = 0; i < hardwareComponents.size(); ++i) {
        if (i > 0) result += "|";
        result += hardwareComponents[i];
    }
    return result;
}

std::string NashApi::GetMotherboardID() {
    try {
        HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
        if (FAILED(hres)) return "";

        std::string serialNumber;

        IWbemLocator* pLoc = NULL;
        hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
            IID_IWbemLocator, (LPVOID*)&pLoc);
        if (SUCCEEDED(hres)) {
            IWbemServices* pSvc = NULL;
            hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pSvc);
            if (SUCCEEDED(hres)) {
                hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                    RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
                if (SUCCEEDED(hres)) {
                    IEnumWbemClassObject* pEnumerator = NULL;
                    hres = pSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT SerialNumber FROM Win32_BaseBoard"),
                        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);

                    if (SUCCEEDED(hres)) {
                        IWbemClassObject* pclsObj = NULL;
                        ULONG uReturn = 0;

                        while (pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn) == S_OK) {
                            VARIANT vtProp;
                            VariantInit(&vtProp);

                            HRESULT hr = pclsObj->Get(L"SerialNumber", 0, &vtProp, 0, 0);
                            if (SUCCEEDED(hr) && vtProp.vt == VT_BSTR) {
                                std::wstring ws(vtProp.bstrVal, SysStringLen(vtProp.bstrVal));
                                serialNumber = std::string(ws.begin(), ws.end());
                                if (serialNumber != "None" && serialNumber != "To be filled by O.E.M.") {
                                    VariantClear(&vtProp);
                                    pclsObj->Release();
                                    break;
                                }
                            }
                            VariantClear(&vtProp);
                            pclsObj->Release();
                        }
                        pEnumerator->Release();
                    }
                }
                pSvc->Release();
            }
            pLoc->Release();
        }

        CoUninitialize();
        return serialNumber;
    }
    catch (...) {
        return "";
    }
}

std::string NashApi::GetGPUId() {
    try {
        DISPLAY_DEVICEA displayDevice;
        displayDevice.cb = sizeof(DISPLAY_DEVICEA);

        std::string gpuInfo;
        for (DWORD i = 0; EnumDisplayDevicesA(NULL, i, &displayDevice, 0); i++) {
            if (displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) {
                gpuInfo = std::string(displayDevice.DeviceString);
                break;
            }
        }

        if (!gpuInfo.empty()) {
            unsigned long hash = 5381;
            for (char c : gpuInfo) {
                hash = ((hash << 5) + hash) + c;
            }
            return std::to_string(hash);
        }

        return "";
    }
    catch (...) {
        return "";
    }
}

std::string NashApi::GetRAMId() {
    try {
        MEMORYSTATUSEX memoryStatus;
        memoryStatus.dwLength = sizeof(memoryStatus);

        if (GlobalMemoryStatusEx(&memoryStatus)) {
            return "MEM_" + std::to_string(memoryStatus.ullTotalPhys);
        }

        return "";
    }
    catch (...) {
        return "";
    }
}

std::string NashApi::GetFallbackHWID() {
    try {
        std::vector<std::string> components;

        char computerName[MAX_PATH];
        DWORD size = MAX_PATH;
        if (GetComputerNameA(computerName, &size)) {
            components.push_back("MACHINE:" + std::string(computerName));
        }

        char userName[MAX_PATH];
        size = MAX_PATH;
        if (GetUserNameA(userName, &size)) {
            components.push_back("USER:" + std::string(userName));
        }

        std::string volumeId = GetVolumeId();
        if (!volumeId.empty()) {
            components.push_back("VOL:" + volumeId);
        }

        std::string result;
        for (size_t i = 0; i < components.size(); ++i) {
            if (i > 0) result += "|";
            result += components[i];
        }

        if (result.empty()) {
            srand(static_cast<unsigned int>(time(NULL)));
            result = "GENERIC_HWID_" + std::to_string(rand());
        }

        return result;
    }
    catch (...) {
        return "FALLBACK_HWID";
    }
}

std::string NashApi::GetVolumeId() {
    try {
        DWORD serialNumber = 0;
        if (GetVolumeInformationA("C:\\", NULL, 0, &serialNumber, NULL, NULL, NULL, 0)) {
            return std::to_string(serialNumber);
        }
        return "";
    }
    catch (...) {
        return "";
    }
}

std::string NashApi::toLower(const std::string& str) {
    try {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    catch (...) {
        return str;
    }
}

void NashApi::ParseResponse(const std::string& response, const std::string& operation) {
    ApiResponse result;
    result.rawResponse = response;

    if (response.empty()) {
        result.success = false;
        result.message = "Empty response from server";
        SetApiResponse(result);
        return;
    }

    try {
        json j = json::parse(response);

        if (operation == "Setup") {
            std::string responseLower = toLower(response);

            bool isSuccess = (response.find("\"success\":true") != std::string::npos) ||
                (responseLower.find("online") != std::string::npos) ||
                (responseLower.find("initialized") != std::string::npos) ||
                (responseLower.find("active") != std::string::npos) ||
                (responseLower.find("ok") != std::string::npos) ||
                (response.find("200") != std::string::npos);

            result.success = isSuccess;

            if (j.contains("message")) {
                result.message = j["message"].get<std::string>();
            }
            else if (isSuccess) {
                result.message = "API initialized successfully";
            }
            else {
                result.message = "Setup completed with unknown response";
            }
        }
        else if (j.contains("success")) {
            result.success = j["success"].get<bool>();
        }

        if (j.contains("message")) {
            result.message = j["message"].get<std::string>();

            if (!j.contains("success") && operation != "Setup") {
                std::string messageLower = toLower(result.message);

                bool isSuccess = (messageLower.find("sucesso") != std::string::npos) ||
                    (messageLower.find("success") != std::string::npos) ||
                    (messageLower.find("bem-sucedida") != std::string::npos) ||
                    (messageLower.find("autenticacao") != std::string::npos) ||
                    (messageLower.find("conta criada") != std::string::npos) ||
                    (messageLower.find("online") != std::string::npos) ||
                    (messageLower.find("login bem-sucedido") != std::string::npos);

                result.success = isSuccess;
            }
        }
        else {
            if (!j.contains("success")) {
                result.success = false;
                result.message = "Response without clear message";
            }
        }

        if (result.success && operation != "Setup" && j.contains("user")) {
            json user = j["user"];
            result.username = user.value("username", "");
            result.productName = user.value("productName", "");
            result.daysRemaining = user.value("daysRemaining", 0);
            result.timesUsed = user.value("timesUsed", 0);
            result.maxUsers = user.value("maxUsers", 0);
            result.isBanned = user.value("isBanned", false);
            result.expirationDate = user.value("expirationDate", "");
        }

        SetApiResponse(result);
    }
    catch (const std::exception& e) {
        result.success = false;
        result.message = "JSON parse error: " + std::string(e.what());
        SetApiResponse(result);
    }
    catch (...) {
        result.success = false;
        result.message = "Unknown parse error";
        SetApiResponse(result);
    }
}

NashApi::NashApi(const std::string& baseUrl) : apiUrl(baseUrl) {
    if (!apiUrl.empty() && apiUrl.back() == '/') {
        apiUrl.pop_back();
    }
    currentResponse.success = false;
    currentResponse.message = "API not initialized";
}

NashApi::~NashApi() {
    CleanupCurl();
}

ApiResponse NashApi::GetApiResponse() {
    std::lock_guard<std::mutex> lock(responseMutex);
    return currentResponse;
}

void NashApi::SetApiResponse(const ApiResponse& response) {
    std::lock_guard<std::mutex> lock(responseMutex);
    currentResponse = response;
}

bool NashApi::Setup() {
    ApiResponse initResult;
    initResult.success = false;
    initResult.message = "Initializing...";
    SetApiResponse(initResult);

    std::string endpoints[] = { "/init", "/status", "/" };
    bool success = false;

    for (const auto& endpoint : endpoints) {
        std::string response = PerformRequest(endpoint);
        ParseResponse(response, "Setup");

        ApiResponse resp = GetApiResponse();
        if (resp.success) {
            success = true;
            break;
        }
    }

    if (!success) {
        ApiResponse failResult;
        failResult.success = false;
        failResult.message = "All endpoints failed. Check internet connection.";
        SetApiResponse(failResult);
    }

    return success;
}

bool NashApi::PerformLogin(const std::string& username, const std::string& password) {
    if (!InitializeCurl()) {
        ApiResponse resp;
        resp.success = false;
        resp.message = "CURL not initialized";
        SetApiResponse(resp);
        return false;
    }

    if (username.empty() || password.empty()) {
        ApiResponse resp;
        resp.success = false;
        resp.message = "Username and password are required";
        SetApiResponse(resp);
        return false;
    }

    try {
        std::string hwid = GetHWID();
        std::string ip = GetIPAddress();

        json body;
        body["username"] = username;
        body["password"] = password;
        body["hwid"] = hwid;
        body["ipAddress"] = ip;

        std::string response = PerformRequest("/auth/login", body.dump());
        ParseResponse(response, "Login");

        return GetApiResponse().success;
    }
    catch (...) {
        ApiResponse resp;
        resp.success = false;
        resp.message = "Login request failed";
        SetApiResponse(resp);
        return false;
    }
}

bool NashApi::PerformRegister(const std::string& username, const std::string& password, const std::string& license) {
    if (!InitializeCurl()) {
        ApiResponse resp;
        resp.success = false;
        resp.message = "CURL not initialized";
        SetApiResponse(resp);
        return false;
    }

    if (username.empty() || password.empty() || license.empty()) {
        ApiResponse resp;
        resp.success = false;
        resp.message = "Username, password and license are required";
        SetApiResponse(resp);
        return false;
    }

    try {
        std::string hwid = GetHWID();
        std::string ip = GetIPAddress();

        json body;
        body["username"] = username;
        body["password"] = password;
        body["license"] = license;
        body["hwid"] = hwid;
        body["ipAddress"] = ip;

        std::string response = PerformRequest("/auth/register", body.dump());
        ParseResponse(response, "Register");

        return GetApiResponse().success;
    }
    catch (...) {
        ApiResponse resp;
        resp.success = false;
        resp.message = "Register request failed";
        SetApiResponse(resp);
        return false;
    }
}

bool NashApi::PerformLoginWithLicense(const std::string& license) {
    if (!InitializeCurl()) {
        ApiResponse resp;
        resp.success = false;
        resp.message = "CURL not initialized";
        SetApiResponse(resp);
        return false;
    }

    if (license.empty()) {
        ApiResponse resp;
        resp.success = false;
        resp.message = "License is required";
        SetApiResponse(resp);
        return false;
    }

    try {
        std::string hwid = GetHWID();
        std::string ip = GetIPAddress();

        json body;
        body["license"] = license;
        body["hwid"] = hwid;
        body["ipAddress"] = ip;

        std::string response = PerformRequest("/auth/login-license", body.dump());
        ParseResponse(response, "LoginLicense");

        return GetApiResponse().success;
    }
    catch (...) {
        ApiResponse resp;
        resp.success = false;
        resp.message = "License login request failed";
        SetApiResponse(resp);
        return false;
    }
}

bool InitializeNashApi() {
    try {
        return g_NashApi.Setup();
    }
    catch (...) {
        return false;
    }
}