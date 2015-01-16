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

BitmapLayer *top_left_layer, *top_right_layer, *bottom_left_layer, *bottom_right_layer;
GBitmap *top_left, *top_right, *bottom_left, *bottom_right;
#define TOTAL_DIGITS 10
const int TIME_IMAGE_RESOURCE_IDS[] = {
	RESOURCE_ID_BLOCK_0,
	RESOURCE_ID_BLOCK_1,
	RESOURCE_ID_BLOCK_2,
	RESOURCE_ID_BLOCK_3,
	RESOURCE_ID_BLOCK_4,
	RESOURCE_ID_BLOCK_5,
	RESOURCE_ID_BLOCK_6,
	RESOURCE_ID_BLOCK_7,
	RESOURCE_ID_BLOCK_8,
	RESOURCE_ID_BLOCK_9
};

static AppTimer *timer;

static void set_container_image(GBitmap **bmp_image, BitmapLayer *bmp_layer, const int resource_id) {
  GBitmap *old_image = *bmp_image;
  *bmp_image = gbitmap_create_with_resource(resource_id);
  bitmap_layer_set_bitmap(bmp_layer, *bmp_image);
  if (old_image != NULL) {
  gbitmap_destroy(old_image);
  old_image = NULL;
  }
}

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

unsigned short get_display_hour(unsigned short hour) {
  if (clock_is_24h_style()) {
    return hour;
  }
  unsigned short display_hour = hour % 12;
  // Converts "0" to "12"
  return display_hour ? display_hour : 12;
}


void handle_tick(struct tm *tick_time, TimeUnits units){
	//APP_LOG(APP_LOG_LEVEL_INFO, "%d free", (int)heap_bytes_free());
	unsigned short display_hour = get_display_hour(tick_time->tm_hour);
	
	
  set_container_image(&top_left, top_left_layer, TIME_IMAGE_RESOURCE_IDS[display_hour/10]);
  set_container_image(&top_right, top_right_layer, TIME_IMAGE_RESOURCE_IDS[display_hour%10]);

  set_container_image(&bottom_left, bottom_left_layer, TIME_IMAGE_RESOURCE_IDS[tick_time->tm_min/10]);
  set_container_image(&bottom_right, bottom_right_layer, TIME_IMAGE_RESOURCE_IDS[tick_time->tm_min%10]);
	
	
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
	
  //APP_LOG(APP_LOG_LEVEL_INFO, "%d free", (int)heap_bytes_free());

	
	//top_left = gbitmap_create_with_resource(TIME_IMAGE_RESOURCE_IDS[8]);
	top_left_layer = bitmap_layer_create(GRect(0, 0, 72, 84));
  bitmap_layer_set_compositing_mode(top_left_layer, GCompOpAnd);
  //bitmap_layer_set_bitmap(top_left_layer, top_left);
  layer_add_child(window_layer, bitmap_layer_get_layer(top_left_layer));	
	
	//top_right = gbitmap_create_with_resource(TIME_IMAGE_RESOURCE_IDS[7]);
	top_right_layer = bitmap_layer_create(GRect(72, 0, 72, 84));
  bitmap_layer_set_compositing_mode(top_right_layer, GCompOpAnd);
  //bitmap_layer_set_bitmap(top_right_layer, top_right);
  layer_add_child(window_layer, bitmap_layer_get_layer(top_right_layer));	
	
	//bottom_left = gbitmap_create_with_resource(TIME_IMAGE_RESOURCE_IDS[9]);
	bottom_left_layer = bitmap_layer_create(GRect(0, 84, 72, 84));
  bitmap_layer_set_compositing_mode(bottom_left_layer, GCompOpAnd);
  //bitmap_layer_set_bitmap(bottom_left_layer, bottom_left);
  layer_add_child(window_layer, bitmap_layer_get_layer(bottom_left_layer));	
	
	//bottom_right = gbitmap_create_with_resource(TIME_IMAGE_RESOURCE_IDS[0]);
	bottom_right_layer = bitmap_layer_create(GRect(72, 84, 72, 84));
  bitmap_layer_set_compositing_mode(bottom_right_layer, GCompOpAnd);
  //bitmap_layer_set_bitmap(bottom_right_layer, bottom_right);
  layer_add_child(window_layer, bitmap_layer_get_layer(bottom_right_layer));	
	
  //APP_LOG(APP_LOG_LEVEL_INFO, "%d free", (int)heap_bytes_free());

  //APP_LOG(APP_LOG_LEVEL_INFO, "%d for resource", sizeof(mask_image));

  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);  
  handle_tick(tick_time, MINUTE_UNIT);	
  tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);

  window_stack_push(window, true);

  timer = app_timer_register(timeout_ms, timer_callback, NULL);
}

static void deinit(void) {
  
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
