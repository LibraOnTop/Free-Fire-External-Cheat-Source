#pragma once
#define CURL_STATICLIB
#include "Auth\Curl\curl.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <atlsecurity.h>
#include "json.hpp"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Normaliz.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Wldap32.lib")

class c_api {
private:
    using json = nlohmann::json;
    const std::string keyauth_api = ("https://keyauth.win/api/1.3/");
    const std::string name = ("viperclient");
    const std::string ownerid = ("eiqCqtjcJZ");
    const std::string version = ("1.0");
    std::string sessionid;

    static inline auto write_callback(void* contents, size_t size, size_t nmemb, void* userp) -> size_t {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    static std::string get_hwid() {
        ATL::CAccessToken accessToken;
        ATL::CSid currentUserSid;
        if (accessToken.GetProcessToken(TOKEN_READ | TOKEN_QUERY) &&
            accessToken.GetUser(&currentUserSid))
            return std::string(CT2A(currentUserSid.Sid()));
        return "none";
    }

    std::string perform_request(const std::string& postfields) {
        std::string response;
        CURL* hnd = curl_easy_init();
        if (!hnd) {
            return ("failed to start connection.");
        }

        std::string url = keyauth_api;

        curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, ("POST"));
        curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Content-Type: application/x-www-form-urlencoded"));
        curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &response);

        curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, postfields.c_str());

        CURLcode ret = curl_easy_perform(hnd);
        curl_slist_free_all(headers);
        curl_easy_cleanup(hnd);

        if (ret != CURLE_OK) {
            return ("failed to make request to server: ") + std::string(curl_easy_strerror(ret));
        }

        return response;
    }

public:
    c_api() {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    ~c_api() {
        curl_global_cleanup();
    }

    inline bool is_session_initialized() const {
        return !sessionid.empty();
    }

    struct stc_client {
        std::string username;
        std::string password;
        std::string hwid;
        struct stc_sub_type {
            bool active;
            std::string expire_date;
        } sub_type;
    } client;

    struct response_t {
        bool success;
        std::string message;
    } response;  // Novo campo para armazenar a resposta de sucesso/erro

    inline auto setup() -> std::string {
        std::string postfields = ("type=init&ver=") + version +
            ("&name=") + name +
            ("&ownerid=") + ownerid;

        std::string response = perform_request(postfields);

        try {
            json response_json = json::parse(response);
            if (response_json["success"].get<bool>()) {
                sessionid = response_json["sessionid"].get<std::string>();
                response_t res = { true, "success" };  // Setando a resposta
                this->response = res;
                return "success";
            }
            response_t res = { false, response_json["message"].get<std::string>() };  // Setando a resposta
            this->response = res;
            return response_json["message"].get<std::string>();
        }
        catch (json::parse_error&) {
            response_t res = { false, "failed to parse server response." };  // Setando a resposta
            this->response = res;
            return "failed to parse server response.";
        }
    }

    inline auto Login(const std::string username, const std::string password) -> std::string {
        if (sessionid.empty()) {
            response_t res = { false, "Session not initialized. Call setup() first." };  // Setando a resposta
            this->response = res;
            return "Session not initialized. Call setup() first.";
        }

        std::string hwid = get_hwid();
        std::string postfields = ("type=login&username=") + username +
            ("&pass=") + password +
            ("&hwid=") + hwid +
            ("&sessionid=") + sessionid +
            ("&name=") + name +
            ("&ownerid=") + ownerid;

        std::string response = perform_request(postfields);

        try {
            json response_json = json::parse(response);
            if (response_json["success"].get<bool>()) {
                client.username = response_json["info"]["username"].get<std::string>();
                client.hwid = hwid;

                if (response_json["info"].contains("subscriptions")) {
                    auto sub = response_json["info"]["subscriptions"][0];
                    client.sub_type.expire_date = sub["expiry"].get<std::string>();
                    client.sub_type.active = true;
                }
                response_t res = { true, "success" };  // Setando a resposta
                this->response = res;
                return "success";
            }
            response_t res = { false, response_json["message"].get<std::string>() };  // Setando a resposta
            this->response = res;
            return response_json["message"].get<std::string>();
        }
        catch (json::parse_error& e) {
            response_t res = { false, "Erro ao processar a resposta do servidor: " + std::string(e.what()) };  // Setando a resposta
            this->response = res;
            return "Erro ao processar a resposta do servidor: " + std::string(e.what());
        }
    }

    inline auto Register_key(const std::string username, const std::string password, const std::string& key) -> std::string {
        if (sessionid.empty()) {
            response_t res = { false, "Session not initialized. Call setup() first." };  // Setando a resposta
            this->response = res;
            return ("Session not initialized. Call setup() first.");
        }

        std::string hwid = get_hwid();
        std::string postfields = ("type=register&username=") + username +
            ("&pass=") + password +
            ("&key=") + key +
            ("&hwid=") + hwid +
            ("&sessionid=") + sessionid +
            ("&name=") + name +
            ("&ownerid=") + ownerid;

        std::string response = perform_request(postfields);

        try {
            json response_json = json::parse(response);
            if (response_json[("success")].get<bool>()) {
                client.username = username;
                client.hwid = hwid;

                if (response_json[("info")].contains(("subscriptions"))) {
                    auto sub = response_json[("info")][("subscriptions")][0];
                    client.sub_type.expire_date = sub[("expiry")].get<std::string>();
                    client.sub_type.active = true;
                }
                response_t res = { true, "success" };  // Setando a resposta
                this->response = res;
                return ("success");
            }
            response_t res = { false, response_json[("message")].get<std::string>() };  // Setando a resposta
            this->response = res;
            return response_json[("message")].get<std::string>();
        }
        catch (json::parse_error& e) {
            response_t res = { false, "Erro ao processar a resposta do servidor: " + std::string(e.what()) };  // Setando a resposta
            this->response = res;
            return ("Erro ao processar a resposta do servidor: ") + std::string(e.what());
        }
    }

    std::string parse_date_dual(const std::string& timestamp) {
        if (timestamp.empty()) return "Invalid date";

        // Converter string -> time_t
        time_t expire_time = std::stoll(timestamp);
        time_t now = std::time(nullptr);

        // Calcular diferença em dias
        double diff_seconds = difftime(expire_time, now);
        int diff_days = static_cast<int>(diff_seconds / (60 * 60 * 24));

        // Converter expiração para struct tm
        std::tm tm_local = {};
        localtime_s(&tm_local, &expire_time);

        char buffer_local[100];
        std::strftime(buffer_local, sizeof(buffer_local), "%d/%m/%Y %H:%M:%S", &tm_local);

        if (diff_days >= 0) {
            return std::to_string(diff_days) + " Day(s)";
        }
        else {
            return "Expired";
        }
    }

};

inline c_api g_Api;
