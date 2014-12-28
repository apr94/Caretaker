#include <pebble.h>

Window* window;
Window* sec_window;

int row;

MenuLayer *menu_layer;
TextLayer *sec_text_layer;
static TextLayer *s_time_layer;

void sec_window_load();
void sec_window_unload();

static void update_time() {
	// Get a tm structure
	time_t temp = time(NULL); 
	struct tm *tick_time = localtime(&temp);

	// Create a long-lived buffer
	static char buffer[] = "00:00";

	// Write the current hours and minutes into the buffer
	if(clock_is_24h_style() == true) {
		//Use 2h hour format
		strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
	} else {
		//Use 12 hour format
		strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
	}

	// Display this time on the TextLayer
	text_layer_set_text(s_time_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	  update_time();
}


void draw_row_callback(GContext *ctx, Layer *cell_layer, MenuIndex *cell_index, void *callback_context)
{
	//Which row is it?
	switch(cell_index->row)
	{
		case 0:
			menu_cell_basic_draw(ctx, cell_layer, "1. Help", "Request for help.", NULL);
			break;
		case 1:
			menu_cell_basic_draw(ctx, cell_layer, "2. Switch", "Dead man's switch.", NULL);
			break;
		case 2:
			menu_cell_basic_draw(ctx, cell_layer, "3. Time", "View date and time.", NULL);
			break;
		case 3:
			menu_cell_basic_draw(ctx, cell_layer, "4. Notifications", "View past notifications.", NULL);
			break;
	}
}

uint16_t num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *callback_context)
{
	return 4;
}

void select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context){

	sec_window = window_create();
	WindowHandlers sec_handlers = {
		.load = sec_window_load,
		.unload = sec_window_unload
	};
	window_set_window_handlers(sec_window, (WindowHandlers) sec_handlers);
	row = cell_index->row;

	window_stack_push(sec_window, true);
}


void window_load(Window *window)
{
	//Create it - 12 is approx height of the top bar
	menu_layer = menu_layer_create(GRect(0, 0, 144, 168 - 16));

	//Let it receive clicks
	menu_layer_set_click_config_onto_window(menu_layer, window);

	//Give it its callbacks
	MenuLayerCallbacks callbacks = {
		.draw_row = (MenuLayerDrawRowCallback) draw_row_callback,
		.get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback) num_rows_callback,
		.select_click = (MenuLayerSelectCallback) select_click_callback
	};
	menu_layer_set_callbacks(menu_layer, NULL, callbacks);

	//Add to Window
	layer_add_child(window_get_root_layer(window), menu_layer_get_layer(menu_layer));
	sec_text_layer = text_layer_create(GRect(0, 0, 144, 168));
}

void create_help_screen(Window *window){

	if(row == 0){
		sec_text_layer = text_layer_create(GRect(0, 0, 144, 168));
		text_layer_set_background_color(sec_text_layer, GColorClear);
		text_layer_set_text_color(sec_text_layer, GColorBlack);
		text_layer_set_text_alignment(sec_text_layer, GTextAlignmentCenter); 
		text_layer_set_text(sec_text_layer, "Request Help");
		layer_add_child(window_get_root_layer(window), text_layer_get_layer(sec_text_layer));
	}
}

void create_switch_screen(Window *window){

	if(row == 1){
		sec_text_layer = text_layer_create(GRect(0, 0, 144, 168));
		text_layer_set_background_color(sec_text_layer, GColorClear);
		text_layer_set_text_color(sec_text_layer, GColorBlack);
		text_layer_set_text_alignment(sec_text_layer, GTextAlignmentCenter); 
		text_layer_set_text(sec_text_layer, "Activate Dead Man Switch");
		layer_add_child(window_get_root_layer(window), text_layer_get_layer(sec_text_layer));
	}
}


void create_time_screen(Window *window){

	if(row == 2){
		sec_text_layer = text_layer_create(GRect(0, 0, 144, 168));
		text_layer_set_background_color(sec_text_layer, GColorClear);
		text_layer_set_text_color(sec_text_layer, GColorBlack);
		text_layer_set_text_alignment(sec_text_layer, GTextAlignmentCenter); 
		text_layer_set_text(sec_text_layer, "Time Display");
		layer_add_child(window_get_root_layer(window), text_layer_get_layer(sec_text_layer));

		text_layer_set_background_color(s_time_layer, GColorClear);
		text_layer_set_text_color(s_time_layer, GColorBlack);
		text_layer_set_text(s_time_layer, "00:00");

		// Improve the layout to be more like a watchface
		text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
		text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

		// Add it as a child layer to the Window's root layer
		layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));

		// Make sure the time is displayed from the start
		update_time();
	}
}

void create_notifications_screen(Window *window){

	if(row == 3){
		sec_text_layer = text_layer_create(GRect(0, 0, 144, 168));
		text_layer_set_background_color(sec_text_layer, GColorClear);
		text_layer_set_text_color(sec_text_layer, GColorBlack);
		text_layer_set_text_alignment(sec_text_layer, GTextAlignmentCenter); 
		text_layer_set_text(sec_text_layer, "Past Notifications");
		layer_add_child(window_get_root_layer(window), text_layer_get_layer(sec_text_layer));
	}
}


void sec_window_load(Window *window){
	switch(row){
		case 0:
			create_help_screen(window);
		case 1:
			create_switch_screen(window);
		case 2:
			create_time_screen(window);
		case 3:
			create_notifications_screen(window);
	}
}


void window_unload(Window *window){
	window_destroy(sec_window);
	menu_layer_destroy(menu_layer);
}

void sec_window_unload(Window *window){
	text_layer_destroy(sec_text_layer);
	text_layer_destroy(s_time_layer);
}

void init()
{
	window = window_create();
	WindowHandlers handlers = {
		.load = window_load,
		.unload = window_unload
	};
	window_set_window_handlers(window, (WindowHandlers) handlers);
	window_stack_push(window, true);


	sec_window = window_create();
	WindowHandlers sec_handlers = {
		.load = sec_window_load,
		.unload = sec_window_unload
	};
	window_set_window_handlers(sec_window, (WindowHandlers) sec_handlers);
	
	// Register with TickTimerService
	//tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

void deinit()
{
	window_destroy(window);
}

int main(void)
{
	init();
	app_event_loop();
	deinit();

	return 0;
}
