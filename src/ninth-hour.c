#include <pebble.h>

static Window *nh_main_window;
static BitmapLayer *nh_background_layer;
static GBitmap *nh_background_bitmap;
/* static GFont nh_time_font; */
static TextLayer *nh_time_layer;
// %d %m
static TextLayer *nh_date_layer;

static void nh_update_time() {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    static char nh_time_buffer[8];
    strftime(nh_time_buffer, sizeof(nh_time_buffer), clock_is_24h_style() ?
             "%H:%M" : "%I:%M", tick_time);

    static char nh_date_buffer[8];
    strftime(nh_date_buffer, sizeof(nh_date_buffer), "%m/%d", tick_time);

    // Update text in layers
    text_layer_set_text(nh_time_layer, nh_time_buffer);
    // For testing specific sizing of certain digits
    // text_layer_set_text(nh_time_layer, "04:40");

    text_layer_set_text(nh_date_layer, nh_date_buffer);
}

static void nh_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    nh_update_time();
}

static void nh_background_load(Layer *window_layer, GRect *bounds) {
    nh_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
    nh_background_layer = bitmap_layer_create(*bounds);
    bitmap_layer_set_bitmap(nh_background_layer, nh_background_bitmap);
    layer_add_child(window_layer, bitmap_layer_get_layer(nh_background_layer));
}

static void nh_time_load(Layer *window_layer, GRect *bounds) {
    /* nh_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_48)); */
    nh_time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58,52), (*bounds).size.w, 50));

    // Make layout more watchface-like
    text_layer_set_background_color(nh_time_layer, GColorClear);
    text_layer_set_text_color(nh_time_layer, GColorWhite);
    text_layer_set_font(nh_time_layer, /* nh_time_font */ fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS));
    text_layer_set_text_alignment(nh_time_layer, GTextAlignmentCenter);

    layer_add_child(window_layer, text_layer_get_layer(nh_time_layer));
}

static void nh_date_load(Layer *window_layer, GRect *bounds) {
    /* nh_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_48)); */
    nh_date_layer = text_layer_create(GRect(6, PBL_IF_ROUND_ELSE(19,13), (*bounds).size.w / 3, 20));

    text_layer_set_background_color(nh_date_layer, GColorClear);
    text_layer_set_text_color(nh_date_layer, GColorWhite);
    text_layer_set_font(nh_date_layer, /* nh_date_font */ fonts_get_system_font(FONT_KEY_GOTHIC_18));
    text_layer_set_text_alignment(nh_date_layer, GTextAlignmentLeft);

    layer_add_child(window_layer, text_layer_get_layer(nh_date_layer));
}

static void nh_main_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    nh_background_load(window_layer, &bounds);

    nh_time_load(window_layer, &bounds);
    nh_date_load(window_layer, &bounds);
}

static void nh_background_unload() {
    gbitmap_destroy(nh_background_bitmap);
    bitmap_layer_destroy(nh_background_layer);
}

static void nh_time_unload() {
    /* fonts_unload_custom_font(nh_time_font); */
    text_layer_destroy(nh_time_layer);
}

static void nh_date_unload() {
    /* fonts_unload_custom_font(nh_date_font); */
    text_layer_destroy(nh_date_layer);
}

static void nh_main_window_unload(Window *window) {
    nh_background_unload();
    nh_time_unload();
    nh_date_unload();
}

static void nh_init() {
    tick_timer_service_subscribe(MINUTE_UNIT, nh_tick_handler);

    nh_main_window = window_create();

    window_set_window_handlers(nh_main_window, (WindowHandlers) {
            .load = nh_main_window_load,
                .unload = nh_main_window_unload
                });

    window_set_background_color(nh_main_window, GColorBlack);
    window_stack_push(nh_main_window, true);
    nh_update_time();
}

static void nh_deinit() {
    window_destroy(nh_main_window);
}

int main(void) {
    nh_init();
    app_event_loop();
    nh_deinit();
}
