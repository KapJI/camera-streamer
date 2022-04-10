#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct buffer_s buffer_t;
typedef struct buffer_list_s buffer_list_t;
typedef struct device_s device_t;
struct pollfd;

typedef struct device_hw_s {
  int (*device_open)(device_t *dev);
  void (*device_close)(device_t *dev);
  int (*device_set_decoder_start)(device_t *dev, bool do_on);
  int (*device_video_force_key)(device_t *dev);
  int (*device_set_fps)(device_t *dev, int desired_fps);
  int (*device_set_option)(device_t *dev, const char *key, const char *value);

  int (*buffer_open)(buffer_t *buf);
  void (*buffer_close)(buffer_t *buf);
  int (*buffer_enqueue)(buffer_t *buf, const char *who);
  int (*buffer_list_dequeue)(buffer_list_t *buf_list, buffer_t **bufp);
  int (*buffer_list_pollfd)(buffer_list_t *buf_list, struct pollfd *pollfd, bool can_dequeue);

  int (*buffer_list_set_format)(buffer_list_t *buf_list, unsigned width, unsigned height, unsigned format, unsigned bytesperline);
  int (*buffer_list_set_buffers)(buffer_list_t *buf_list, int nbufs);
  int (*buffer_list_set_stream)(buffer_list_t *buf_list, bool do_on);
} device_hw_t;

typedef struct device_s {
  char *name;
  char *path;
  char bus_info[64];
  bool allow_dma;

  device_hw_t *hw;
  buffer_list_t *capture_list;
  buffer_list_t *output_list;

  union {
    struct device_v4l2_s *v4l2;
  };

  device_t *output_device;
  bool paused;
  bool decoder_started;
} device_t;

device_t *device_open(const char *name, const char *path, device_hw_t *hw);
void device_close(device_t *device);

int device_open_buffer_list(device_t *dev, bool do_capture, unsigned width, unsigned height, unsigned format, unsigned bytesperline, int nbufs, bool do_mmap);
int device_open_buffer_list_output(device_t *dev, buffer_list_t *capture_list);
int device_open_buffer_list_capture(device_t *dev, buffer_list_t *output_list, float div, unsigned format, bool do_mmap);
int device_consume_event(device_t *device);

int device_set_stream(device_t *dev, bool do_on);
int device_set_decoder_start(device_t *dev, bool do_on);
int device_video_force_key(device_t *dev);

int device_set_fps(device_t *dev, int desired_fps);
int device_set_option_string(device_t *dev, const char *option, const char *value);
void device_set_option_list(device_t *dev, const char *option_list);

device_t *device_v4l2_open(const char *name, const char *path);