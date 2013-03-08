#include "tool_server.h"

#include <Windows.h>
#include <gl/GL.h>
#include <math.h>
#include <stdio.h>

#include "gpr_types.h"
#include "gpr_assert.h"

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '0', '1', '2', '3',
  '4', '5', '6', '7', '8', '9', '+', '/'};
static int mod_table[] = {0, 2, 1};

static void base64_encode(const unsigned char *data, U32 input_length,
                          unsigned char *out, U32 *output_length)
{
  *output_length = (size_t) (4.0 * ceil((double) input_length / 3.0));

  for (int i = 0, j = 0; i < input_length;) {

    uint32_t octet_a = i < input_length ? data[i++] : 0;
    uint32_t octet_b = i < input_length ? data[i++] : 0;
    uint32_t octet_c = i < input_length ? data[i++] : 0;

    uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

    out[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
    out[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
    out[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
    out[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
  }

  for (int i = 0; i < mod_table[input_length % 3]; i++)
    out[*output_length - 1 - i] = '=';
}

static int callback_engine(struct libwebsocket_context *context,
                           struct libwebsocket *wsi,
                           enum libwebsocket_callback_reasons reason, 
                           void *user, void *in, size_t len)
{
  tool_server_t *server;
  switch(reason)
  {
  case LWS_CALLBACK_SERVER_WRITEABLE:
    server = (tool_server_t*)libwebsocket_context_user(context);
    libwebsocket_write(wsi, server->image, server->image_size, LWS_WRITE_TEXT);
    break;
  default : break;
  }
  return 0;
}

static struct libwebsocket_protocols protocols[] = {
  // first protocol must always be HTTP handler
  { "engine-protocol", callback_engine,  0 },
  { NULL, NULL, 0 }
};

void tool_server_init(tool_server_t *server, window_manager *wmanager,
                      gpr_allocator_t *allocator)
{
  U32 buf_size = wmanager->width*wmanager->height*3;

  server->allocator = allocator;
  server->width     = wmanager->width;
  server->height    = wmanager->height;
  server->out_buf   = (unsigned char*)gpr_allocate(allocator, buf_size
                        + LWS_SEND_BUFFER_PRE_PADDING + LWS_SEND_BUFFER_POST_PADDING);
  server->tmp_buf   = (unsigned char*)gpr_allocate(allocator, tjBufSize(server->width, server->height, TJSAMP_444));
  server->image     = server->out_buf + LWS_SEND_BUFFER_PRE_PADDING;

  {
    memset(&server->context_info, 0, sizeof(server->context_info));
    server->context_info.port      = 9000;
    server->context_info.protocols = protocols;
    server->context_info.user      = server;

    server->context = libwebsocket_create_context(&server->context_info);
    gpr_assert_msg(server->context != NULL, "websocket context creation failed");
  }

  server->tjh = tjInitTransform();
}

void tool_server_shutdown(tool_server_t *server)
{
  libwebsocket_context_destroy(server->context);
  gpr_deallocate(server->allocator, server->out_buf);
  gpr_deallocate(server->allocator, server->tmp_buf);
  tjDestroy(server->tjh);
}

void tool_server_update(tool_server_t *server)
{
  glReadPixels(0, 0, server->width, server->height, GL_RGB, GL_UNSIGNED_BYTE, server->image);

  tjCompress2(server->tjh, server->image, 
    server->width, 0,
    server->height, 
    TJPF_RGB,
    &server->tmp_buf,
    (unsigned long*)&server->image_size,
    TJSAMP_444,     //jpegSubsamp no chrominance subsampling
    100,            //image quality (1 = worst,100 = best)
    TJFLAG_BOTTOMUP | TJFLAG_NOREALLOC //flags the bitwise OR of one or more of the @ref TJFLAG_BOTTOMUP "flags"
  );

  base64_encode(server->tmp_buf, server->image_size, server->image, &server->image_size);
  libwebsocket_callback_on_writable_all_protocol(&protocols[0]);
  libwebsocket_service(server->context, 0);
}