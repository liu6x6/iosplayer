#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

void http_client_init();
void http_client_cleanup();
void send_tap_request(double x, double y);
void send_swipe_request(double fromX, double fromY, double toX, double toY, double duration);
void send_keys_request(const char* text);

#endif // HTTP_CLIENT_H
