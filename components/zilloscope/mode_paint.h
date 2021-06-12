#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/web_server_base/web_server_base.h"
#include "zilloscope_mode.h"

namespace esphome {
namespace zilloscope {

struct UrlMatch {
  std::string domain;  ///< The domain of the component, for example "sensor"
  std::string id;      ///< The id of the device that's being accessed, for example "living_room_fan"
  std::string method;  ///< The method that's being called, for example "turn_on"
  bool valid;          ///< Whether this match is valid
};

class ModePaint: public Mode, public Component, public AsyncWebHandler  {
  public:
    ModePaint(const std::string &name,
              web_server_base::WebServerBase *base
              )
    : Mode(name,nullptr,16) , base_(base) {}

    void setup() {
      this->base_->init();
    }

    void loop() {
    }

    UrlMatch match_url(const std::string &url, bool only_domain = false) {
      UrlMatch match;
      match.valid = false;
      size_t domain_end = url.find('/', 1);
      if (domain_end == std::string::npos)
        return match;
      match.domain = url.substr(1, domain_end - 1);
      if (only_domain) {
        match.valid = true;
        return match;
      }
      if (url.length() == domain_end - 1)
        return match;
      size_t id_begin = domain_end + 1;
      size_t id_end = url.find('/', id_begin);
      match.valid = true;
      if (id_end == std::string::npos) {
        match.id = url.substr(id_begin, url.length() - id_begin);
        return match;
      }
      match.id = url.substr(id_begin, id_end - id_begin);
      size_t method_begin = id_end + 1;
      match.method = url.substr(method_begin, url.length() - method_begin);
      return match;
    }

    bool canHandle(AsyncWebServerRequest *request) {
      if(request->url() == "/paint")
        return true;

      //#ifdef WEBSERVER_CSS_INCLUDE
      //  if (request->url() == "/paint.css")
      //    return true;
      //#endif

      //#ifdef WEBSERVER_JS_INCLUDE
      //  if (request->url() == "/paint.js")
      //    return true;
      //#endif

      UrlMatch match = match_url(request->url().c_str(), true);
      if(!match.valid)
        return false;


      if(request->url() == "/paint/getbuffer")
        return true;
      if(request->url() == "/paint/setbuffer")
        return true;
      if(request->url() == "/paint/setpixelat")
        return true;
      if(request->url() == "/paint/getpixelat")
        return true;

      return false;
    }

  protected:
    web_server_base::WebServerBase *base_;
  };

}
}

