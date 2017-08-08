extern "C" {
#include "./mongoose/mongoose.h"
}

#include <string>
#include <fstream>
#include <streambuf>
#include <strstream>
#include <map>
#include <chrono>
#include <cstdlib>

static const char *s_http_port = "8000";
static struct mg_serve_http_opts opts;
static int count = 0;
static std::strstream events;
static std::string content_part_html;
static std::map<int,std::chrono::system_clock::time_point> started_time;

static void ev_handler(struct mg_connection *c, int ev, void *p) {
  if (ev == MG_EV_HTTP_REQUEST) {
    struct http_message *hm = (struct http_message *) p;

    if (0 == strncmp("POST", hm->method.p, 4)) {
        if (0 == strncmp("/start", hm->uri.p, 6)) {
            auto response = std::to_string(++count);
            started_time[count] = std::chrono::system_clock::now();
            mg_send_head(c, 200, response.size(), "Content-Type: text/plain");
            mg_send(c, response.c_str(), response.size());
            events << response << "(" << std::string(hm->query_string.p, hm->query_string.len) << "): started" << std::endl;
        }
        else {
            auto request_id = std::atol(hm->uri.p+1);
            auto pos = started_time.find(request_id);
            std::string result("the request was not found");
            if (pos != started_time.end()) {
                auto start = pos->second;
                auto diff = std::chrono::system_clock::now() - start;
                auto microseconds_count = std::chrono::duration_cast<std::chrono::microseconds>(diff).count();
                result = std::to_string(microseconds_count) + " microseconds";
                events << request_id << ": " << result << std::endl;
                started_time.erase(pos);
            }
            mg_send_head(c, 200, result.size(), "Content-Type: text/plain");
            mg_send(c, result.c_str(), result.size());
        }
    }
    else if (0 == strncmp("/events", hm->uri.p, 7)) {
        mg_send_head(c, 200, events.pcount(), "Content-Type: text/plain");
        mg_send(c, events.str(), events.pcount());
    }
    else if (0 == strncmp("/content-part.html", hm->uri.p, 18)) {
        auto buffer = content_part_html;
        auto pos = buffer.find("{{request_id}}");
        buffer.replace(pos, 14, std::string(hm->query_string.p, hm->query_string.len));
        mg_send_head(c, 200, buffer.size(), "Content-Type: text/plain");
        mg_send(c, buffer.c_str(), buffer.size());
    }
    else {
        mg_serve_http(c, hm, opts);
    }
  }
}

int main(void) {
  struct mg_mgr mgr;
  struct mg_connection *c;

  mg_mgr_init(&mgr, NULL);
  c = mg_bind(&mgr, s_http_port, ev_handler);
  mg_set_protocol_http_websocket(c);

    std::ifstream t("./web/content-part.html");
    content_part_html = std::string((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());

    memset(&opts, 0, sizeof(opts));  // Reset all options to defaults
    opts.document_root = "./web";       // Serve files from the current directory

  for (;;) {
    mg_mgr_poll(&mgr, 1000);
  }
  mg_mgr_free(&mgr);

  return 0;
}