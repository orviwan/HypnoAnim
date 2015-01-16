#include <pebble.h>

static Window *window;

static const uint32_t timeout_ms = 120;

static int current_frame = 0;
static Layer *anim_layer;

#define TOTAL_FRAMES 6 
BitmapLayer *layer;
GBitmap *current_image;

const int ANIM_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_FRAME_0,
  RESOURCE_ID_FRAME_1,
  RESOURCE_ID_FRAME_2,
  RESOURCE_ID_FRAME_3,
  RESOURCE_ID_FRAME_4,
  RESOURCE_ID_FRAME_5
};

static GBitmap *mask_image;
static BitmapLayer *mask_layer;

static AppTimer *timer;

void change_background() {
	gbitmap_destroy(current_image);
	if(current_frame >= TOTAL_FRAMES) {
		current_frame = 0;
	}
	current_image = gbitmap_create_with_resource(ANIM_IMAGE_RESOURCE_IDS[current_frame]);
	bitmap_layer_set_bitmap(layer, current_image);
	current_frame++;
}

static void update_image_layer(Layer *layer, GContext* ctx) {
	change_background();
}

static void timer_callback(void *context) {
  layer_mark_dirty(anim_layer);
  timer = app_timer_register(timeout_ms, timer_callback, NULL);
}

void test(struct tm *t, TimeUnits units){
	APP_LOG(APP_LOG_LEVEL_INFO, "%d free", (int)heap_bytes_free());
}

static void init(void) {
  window = window_create();
  Layer *window_layer = window_get_root_layer(window);
  GRect frame = GRect(0, 0, 144, 168);
	
  anim_layer = layer_create(frame);
  layer_set_update_proc(anim_layer, update_image_layer);
  layer_add_child(window_layer, anim_layer);
	
  layer = bitmap_layer_create(frame);
  layer_add_child(anim_layer, bitmap_layer_get_layer(layer));
	
  APP_LOG(APP_LOG_LEVEL_INFO, "%d free", (int)heap_bytes_free());
	
  mask_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WB);
  mask_layer = bitmap_layer_create(frame);
  bitmap_layer_set_compositing_mode(mask_layer, GCompOpOr);
  bitmap_layer_set_bitmap(mask_layer, mask_image);
  layer_add_child(window_layer, bitmap_layer_get_layer(mask_layer));
	
  APP_LOG(APP_LOG_LEVEL_INFO, "%d free", (int)heap_bytes_free());

  APP_LOG(APP_LOG_LEVEL_INFO, "%d for resource", sizeof(mask_image));

  tick_timer_service_subscribe(SECOND_UNIT, test);

  window_stack_push(window, true);

  timer = app_timer_register(timeout_ms, timer_callback, NULL);
}

static void deinit(void) {
  
}

int main(void) {
  init();
  app_event_loop();
  //deinit();
}
