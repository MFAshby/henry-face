#include <pebble.h>

typedef struct tm* p_tm;

static Window* s_window;
static GBitmap* s_bitmap;
static BitmapLayer* s_bitmap_layer;

static TextLayer* s_time_layer;
#define time_buf_len 50
static char time_buf[time_buf_len];

static void show_henry(Window* window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  int16_t x = 0;
  int16_t y = bounds.size.h - 141;
  int16_t w = 144;
  int16_t h = 141;

  s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_HENRY);
  s_bitmap_layer = bitmap_layer_create(GRect(x, y, w, h));
  bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpSet);
  bitmap_layer_set_bitmap(s_bitmap_layer, s_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));

  int yshift = 5;
  x = 0;
  y = -yshift;
  w = 144;
  h = bounds.size.h - 141 + yshift;

  s_time_layer = text_layer_create(GRect(x, y, w, h));
  text_layer_set_text(s_time_layer, time_buf);
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void show_time(p_tm tick_time) {  
  strftime(time_buf, time_buf_len, "%H:%M %a %d %b", tick_time);
  layer_mark_dirty(text_layer_get_layer(s_time_layer));
}

static void handle_tick_timer(p_tm tick_time, TimeUnits units_changed) {
  show_time(tick_time);
}

static void prv_window_load(Window *window) {
  show_henry(window);

  // Show time now
  time_t now = time(NULL);
  p_tm tick_time = localtime(&now);
  show_time(tick_time);

  // Subscribe to timer
  tick_timer_service_subscribe(MINUTE_UNIT, handle_tick_timer);
}

static void prv_window_unload(Window *window) {
  gbitmap_destroy(s_bitmap);
  bitmap_layer_destroy(s_bitmap_layer);
  text_layer_destroy(s_time_layer);
}

static void init(void) {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  const bool animated = true;
  window_stack_push(s_window, animated);
}

static void deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
