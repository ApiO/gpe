#ifndef TOOL_SERVER_H
#define TOOL_SERVER_H

#include "libwebsockets/lib/libwebsockets.h"

#include "gpr_memory.h"
#include "gpr_buffer.h"
#include "window_manager.h"

#include "libjpeg-turbo/turbojpeg.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct 
{
  gpr_allocator_t      *allocator;
  struct lws_context_creation_info 
                        context_info;
  libwebsocket_context *context;
  unsigned char        *out_buf;
  unsigned char        *tmp_buf;
  unsigned char        *image;
  U32                   image_size;
  U32                   width, height;
  tjhandle              tjh;
} tool_server_t;

void tool_server_init     (tool_server_t *server, window_manager *wmanager,
                           gpr_allocator_t *allocator);
void tool_server_shutdown (tool_server_t *server);
void tool_server_update   (tool_server_t *server);

#ifdef __cplusplus
}
#endif

#endif // TOOL_SERVER_H