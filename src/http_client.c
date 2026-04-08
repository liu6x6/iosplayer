#include "http_client.h"
#include <stdio.h>
#include <stdlib.h> // For malloc and free
#include <curl/curl.h>
#include <string.h>

#define WDA_BASE_URL "http://localhost:8100/session/MY-Session/wda"

static void perform_request(const char* url, const char* json_payload) {
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if(curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json;charset=UTF-8");

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}

void send_tap_request(double x, double y) {
    char url[256];
    snprintf(url, sizeof(url), "%s/tap", WDA_BASE_URL);

    char json_payload[128];
    snprintf(json_payload, sizeof(json_payload), "{\"x\": %.2f, \"y\": %.2f}", x, y);

    perform_request(url, json_payload);
}

void send_swipe_request(double fromX, double fromY, double toX, double toY, double duration) {
    char url[256];
    snprintf(url, sizeof(url), "%s/drag", WDA_BASE_URL);

    char json_payload[256];
    snprintf(json_payload, sizeof(json_payload), 
             "{\"fromX\": %.2f, \"fromY\": %.2f, \"toX\": %.2f, \"toY\": %.2f, \"duration\": %.2f}", 
             fromX, fromY, toX, toY, duration);

    perform_request(url, json_payload);
}

void send_keys_request(const char* text) {
    char url[256];
    snprintf(url, sizeof(url), "%s/keys", WDA_BASE_URL);

    // WDA expects an array of single-character strings
    size_t text_len = strlen(text);
    size_t json_len = 10 + text_len * 5; // Estimate buffer size: '{"value":[]}' + '"c",' for each char
    char* json_payload = (char*)malloc(json_len);
    if (!json_payload) return;

    strcpy(json_payload, "{\"value\":[");
    for (size_t i = 0; i < text_len; ++i) {
        char segment[8];
        // Handle special characters that need escaping
        if (text[i] == '\\') snprintf(segment, sizeof(segment), "\"\\\\\"");
        else if (text[i] == '"') snprintf(segment, sizeof(segment), "\"\\\"\"");
        else if (text[i] == '\n') snprintf(segment, sizeof(segment), "\"\\n\"");
        else if (text[i] == '\b') snprintf(segment, sizeof(segment), "\"\\b\"");
        else snprintf(segment, sizeof(segment), "\"%c\"", text[i]);
        
        strcat(json_payload, segment);
        if (i < text_len - 1) {
            strcat(json_payload, ",");
        }
    }
    strcat(json_payload, "]}");

    perform_request(url, json_payload);
    free(json_payload);
}

void http_client_init() {
    curl_global_init(CURL_GLOBAL_ALL);
}

void http_client_cleanup() {
    curl_global_cleanup();
}
