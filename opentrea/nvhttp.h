//
// Created by loki on 6/3/19.
//

#ifndef OPENTREA_NVHTTP_H
#define OPENTREA_NVHTTP_H

#include <functional>
#include <string>

#include "thread_safe.h"

#define CA_DIR OPENTREA_ASSETS_DIR "/demoCA"
#define PRIVATE_KEY_FILE CA_DIR    "/cakey.pem"
#define CERTIFICATE_FILE CA_DIR    "/cacert.pem"

namespace nvhttp {
void start(std::shared_ptr<safe::signal_t> shutdown_event);
}

#endif //OPENTREA_NVHTTP_H
