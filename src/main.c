#include <pebble.h>

static Window *window;

static const uint32_t timeout_ms = 120;

static int current_frame = 0;
static Layer *anim_layer;
static TextLayer *text_layer;


#define TOTAL_FRAMES 6 
static GBitmap *anim_images[TOTAL_FRAMES];
static BitmapLayer *anim_layers[TOTAL_FRAMES];

const int ANIM_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_FRAME_0,
  RESOURCE_ID_FRAME_1,
  RESOURCE_ID_FRAME_2,
  RESOURCE_ID_FRAME_3,
  RESOURCE_ID_FRAME_4,
  RESOURCE_ID_FRAME_5
};

static AppTimer *timer;

void change_background() {
	int prev_frame = current_frame-1;
	if(current_frame == 0) {
		prev_frame = TOTAL_FRAMES-1;
	}
	if(current_frame >= TOTAL_FRAMES) {
		current_frame = 0;
	}

	layer_set_hidden(bitmap_layer_get_layer(anim_layers[current_frame]), false);
	layer_set_hidden(bitmap_layer_get_layer(anim_layers[prev_frame]), true);

	current_frame++;
}

static void update_image_layer(Layer *layer, GContext* ctx) {
	change_background();
}

static void timer_callback(void *context) {
  layer_mark_dirty(anim_layer);
  timer = app_timer_register(timeout_ms, timer_callback, NULL);
}

static void init(void) {
  window = window_create();
  Layer *window_layer = window_get_root_layer(window);
  GRect frame = { {0, 0}, {144, 168} };
	
  anim_layer = layer_create(frame);
  layer_set_update_proc(anim_layer, update_image_layer);
  layer_add_child(window_layer, anim_layer);

  for (int i = 0; i < TOTAL_FRAMES; ++i) {
    anim_layers[i] = bitmap_layer_create(frame);
    layer_add_child(anim_layer, bitmap_layer_get_layer(anim_layers[i]));
		anim_images[i] = gbitmap_create_with_resource(ANIM_IMAGE_RESOURCE_IDS[i]);
		bitmap_layer_set_bitmap(anim_layers[i], anim_images[i]);
		layer_set_hidden(bitmap_layer_get_layer(anim_layers[i]), true);
  }
	layer_set_hidden(bitmap_layer_get_layer(anim_layers[TOTAL_FRAMES-1]), false);

  window_stack_push(window, true);

  timer = app_timer_register(timeout_ms, timer_callback, NULL);
}

static void deinit(void) {
  for (int i = 0; i < TOTAL_FRAMES; i++) {
    layer_remove_from_parent(bitmap_layer_get_layer(anim_layers[i]));
    gbitmap_destroy(anim_images[i]);
    anim_images[i] = NULL;
    bitmap_layer_destroy(anim_layers[i]);
    anim_layers[i] = NULL;
  }	
  layer_destroy(anim_layer);
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}