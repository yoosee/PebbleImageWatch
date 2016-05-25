#include <pebble.h>

static Window *s_main_window;

static TextLayer *s_clock_label;
static GFont s_clock_font;

static GBitmap *s_background_bitmap;
static BitmapLayer *s_background_layer;


static void update_time() {
  time_t now = time(NULL);  
  struct tm *t = localtime(&now);
  static char s_time_buffer[10];
  strftime(s_time_buffer, sizeof(s_time_buffer), "%H:%M", t);
  text_layer_set_text(s_clock_label, s_time_buffer);
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  if(tick_time->tm_sec % 20 == 0) {  
    update_time();
    //layer_mark_dirty(window_get_root_layer(s_main_window));
  }
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create GBitmap and add it to Background Layer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND); // Replace image in RESOURCES
  s_background_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
  // Create Text Layer 
  s_clock_label = text_layer_create(GRect(0, 40, bounds.size.w, 40));  
  text_layer_set_background_color(s_clock_label, GColorClear);
  text_layer_set_text_color(s_clock_label, GColorBlack); // Set Clock Color
  text_layer_set_text_alignment(s_clock_label, GTextAlignmentCenter);
  s_clock_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CLOCK_34));
  text_layer_set_font(s_clock_label, s_clock_font);
  layer_add_child(window_layer, text_layer_get_layer(s_clock_label));
  update_time();
}

static void main_window_unload(Window *window) {  
  gbitmap_destroy(s_background_bitmap);
  bitmap_layer_destroy(s_background_layer);  
  
  fonts_unload_custom_font(s_clock_font);
  text_layer_destroy(s_clock_label);
  
}

static void init(void) {
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack); // default background color
    
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload  
  });
  
  window_stack_push(s_main_window, true);
  
  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
