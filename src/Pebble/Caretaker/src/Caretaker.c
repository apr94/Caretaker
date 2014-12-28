#include <pebble.h>

#define FALL_DOWN 1
#define KEY_ALERT 2
#define KEY_LOC 3
#define KEY_BAT 6
#define ALERT_RESULT 4 
#define LOC_RESULT 5 

// Globals declared in the beginning

Window* window;
Window* sec_window;
Window* fall_window;

// Our notifications

char * nots[5] = {"ALERT NOT SENT", "LOCATION NOT SENT", "ALERT SENT", "LOCATION SENT", "FALL DETECTED"};

int not_pointer;
int in_dms_mode;
int row;
int now;
int start;
int end;
int is_fall_screen;
int did_fall;

MenuLayer *menu_layer;
TextLayer *sec_text_layer;
TextLayer *fall_text_layer;
TextLayer *fall_body_layer;
TextLayer *help_body_layer;

TextLayer *not_body1_layer;
TextLayer *not_body2_layer;
TextLayer *not_body3_layer;
TextLayer *dms_body_layer;

TextLayer *text_date_layer;
TextLayer *text_time_layer;
Layer *line_layer;

void config_provider2(Window *window);
void sec_window_load();
void sec_window_unload();

// Push msg to msg queue, only the top three are saved

void push_msg(char * msg){
    text_layer_set_text(not_body3_layer, text_layer_get_text(not_body2_layer));
    text_layer_set_text(not_body2_layer, text_layer_get_text(not_body1_layer));
   text_layer_set_text(not_body1_layer, msg);
}

// send alert via the appmessage protocol

void send_alert(){
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    // Add a key-value pair
    dict_write_uint8(iter, KEY_ALERT, 1);

    // Send the message!
    app_message_outbox_send();
}

// send updated location via appmessage protocol

void update_loc(){
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    // Add a key-value pair
    int batlevel;
    batlevel = battery_state_service_peek().charge_percent;
    dict_write_uint8(iter, KEY_LOC, batlevel);
    // Send the message!
    app_message_outbox_send();
}

//handle a message from the worker. A background worker handles the fall detection logic

static void worker_message_handler(uint16_t type, AppWorkerMessage *data) {
    if (type == FALL_DOWN) { 
	if(is_fall_screen == 0){
	    window_stack_push(fall_window, true);
	}
    }
}

// callback to handle time screen display

void line_layer_update_callback(Layer *layer, GContext* ctx) {
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
    // Need to be static because they're used by the system later.
    static char time_text[] = "00:00";
    static char date_text[] = "Xxxxxxxxx 00";
    char *time_format;
    if (!tick_time) {
	time_t now = time(NULL);
	tick_time = localtime(&now);
    }
    // TODO: Only update the date when it's changed.
    strftime(date_text, sizeof(date_text), "%B %e", tick_time);
    text_layer_set_text(text_date_layer, date_text);
    if (clock_is_24h_style()) {
	time_format = "%R";
    } else {
	time_format = "%I:%M";
    }
    strftime(time_text, sizeof(time_text), time_format, tick_time);
    // Kludge to handle lack of non-padded hour format string
    // for twelve hour clock.
    if (!clock_is_24h_style() && (time_text[0] == '0')) {
	memmove(time_text, &time_text[1], sizeof(time_text) - 1);
    }

    if(tick_time->tm_sec < 1){
	update_loc();
    }

    now = tick_time->tm_hour * 3600 + tick_time->tm_min * 60 + tick_time->tm_sec;
    text_layer_set_text(text_time_layer, time_text);

    if(is_fall_screen && did_fall){
	if(start == -1){
	    start = now;
	    end = start + 5;
	}
	if( end >= now){

	    // char text[80]; 
	    //snprintf(text, sizeof(text), "FALL DETECTED! ALERT WILL BE SENT TO CARETAKER IN %d SECONDS. CLICK THE SELECT BUTTON TO CANCEL.", end-now);
	    // snprintf(text, sizeof(text), "%d", 4);

	    // Countdown for the fall sequence

	    char * c;
	    if(end-now >= 5){
		c = "FALL DETECTED! ALERT WILL BE SENT TO CARETAKER IN 5 SECONDS. CLICK THE SELECT BUTTON TO CANCEL.";	
	    }
	    else if(end-now >= 4){
		c = "FALL DETECTED! ALERT WILL BE SENT TO CARETAKER IN 4 SECONDS. CLICK THE SELECT BUTTON TO CANCEL.";	
	    }
	    else if(end-now >= 3){
		c = "FALL DETECTED! ALERT WILL BE SENT TO CARETAKER IN 3 SECONDS. CLICK THE SELECT BUTTON TO CANCEL.";	
	    }
	    else if(end-now >= 2){
		c = "FALL DETECTED! ALERT WILL BE SENT TO CARETAKER IN 2 SECONDS. CLICK THE SELECT BUTTON TO CANCEL.";	
	    }
	    else if(end-now >= 1){
		c = "FALL DETECTED! ALERT WILL BE SENT TO CARETAKER IN 1 SECOND. CLICK THE SELECT BUTTON TO CANCEL.";	
	    }
	    else{
		c = "ALERT SENT TO CARETAKER. HELP IS ON THE WAY.";
		send_alert();
		did_fall = 2;	
	    }

	    text_layer_set_text(fall_body_layer, c);
	}
    }
    else{
	start = -1;
	end = -1;
    }
}

// function to handle drawing the initial menu

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

// callbacks to load and unload windows off the stack

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
    window_set_click_config_provider(sec_window, (ClickConfigProvider) config_provider2);
    row = cell_index->row;
    window_stack_push(sec_window, true);
}

// when the main window loads

void window_load(Window *window)
{
    not_body1_layer = text_layer_create(GRect(0, 60, 144, 168-60));
    not_body2_layer = text_layer_create(GRect(0, 90, 144, 168-90));
    not_body3_layer = text_layer_create(GRect(0, 120, 144, 168-120));

    text_layer_set_text(not_body1_layer, "-");
    text_layer_set_text(not_body2_layer, "-");
    text_layer_set_text(not_body3_layer, "-");
    
    help_body_layer = text_layer_create(GRect(0, 60, 144, 168-60));
    dms_body_layer = text_layer_create(GRect(0, 60, 144, 168-60));

    text_date_layer = text_layer_create(GRect(8, 68, 144-8, 168-68));
    text_time_layer = text_layer_create(GRect(7, 92, 144-7, 168-92));

    fall_body_layer = text_layer_create(GRect(7, 40, 144-7, 168-40));

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
}

// functions to create each of the four screens

void create_help_screen(Window *window){

    if(row == 0){
	sec_text_layer = text_layer_create(GRect(0, 0, 144, 168));
	text_layer_set_font(sec_text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
	text_layer_set_background_color(sec_text_layer, GColorClear);
	text_layer_set_text_color(sec_text_layer, GColorBlack);
	text_layer_set_text_alignment(sec_text_layer, GTextAlignmentCenter); 
	text_layer_set_text(sec_text_layer, "Request Help");


	text_layer_set_background_color(help_body_layer, GColorClear);
	text_layer_set_text_color(help_body_layer, GColorBlack);
	text_layer_set_text_alignment(help_body_layer, GTextAlignmentCenter); 
	text_layer_set_text(help_body_layer, "AN ALERT FOR HELP HAS BEEN SENT TO THE PRIMARY CARETAKER.");
	send_alert();
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(sec_text_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(help_body_layer));
    }
}

void create_switch_screen(Window *window){

    if(row == 1){
	sec_text_layer = text_layer_create(GRect(0, 0, 144, 168));
	text_layer_set_font(sec_text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
	text_layer_set_background_color(sec_text_layer, GColorClear);
	text_layer_set_text_color(sec_text_layer, GColorBlack);
	text_layer_set_text_alignment(sec_text_layer, GTextAlignmentCenter); 
	text_layer_set_text(sec_text_layer, "Activate Dead Man Switch");

	text_layer_set_background_color(dms_body_layer, GColorClear);
	text_layer_set_text_color(dms_body_layer, GColorBlack);
	text_layer_set_text_alignment(dms_body_layer, GTextAlignmentCenter); 

	in_dms_mode = 0;
	text_layer_set_text(dms_body_layer, "PRESS AND HOLD UP BUTTON TO ENTER DEAD MAN SWITCH MODE");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(sec_text_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(dms_body_layer));
    }
}


void create_time_screen(Window *window){

    if(row == 2){

	Layer *window_layer = window_get_root_layer(window);

	sec_text_layer = text_layer_create(GRect(0, 0, 144, 168));
	text_layer_set_font(sec_text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
	text_layer_set_background_color(sec_text_layer, GColorClear);
	text_layer_set_text_color(sec_text_layer, GColorBlack);
	text_layer_set_text_alignment(sec_text_layer, GTextAlignmentCenter); 
	text_layer_set_text(sec_text_layer, "Time Display");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(sec_text_layer));

	text_layer_set_text_color(text_date_layer, GColorBlack);
	text_layer_set_background_color(text_date_layer, GColorClear);
	text_layer_set_font(text_date_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
	layer_add_child(window_layer, text_layer_get_layer(text_date_layer));

	text_layer_set_text_color(text_time_layer, GColorBlack);
	text_layer_set_background_color(text_time_layer, GColorClear);
	text_layer_set_font(text_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
	layer_add_child(window_layer, text_layer_get_layer(text_time_layer));

	GRect line_frame = GRect(8, 97, 125, 2);
	line_layer = layer_create(line_frame);
	layer_set_update_proc(line_layer, line_layer_update_callback);
	layer_add_child(window_layer, line_layer);

	handle_minute_tick(NULL, MINUTE_UNIT);
    }
}

void create_notifications_screen(Window *window){

    if(row == 3){
	sec_text_layer = text_layer_create(GRect(0, 0, 144, 168));
	text_layer_set_font(sec_text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));		
	text_layer_set_background_color(sec_text_layer, GColorClear);
	text_layer_set_text_color(sec_text_layer, GColorBlack);
	text_layer_set_text_alignment(sec_text_layer, GTextAlignmentCenter); 
	text_layer_set_text(sec_text_layer, "Past Notifications");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(sec_text_layer));

	text_layer_set_background_color(not_body1_layer, GColorClear);
	text_layer_set_text_color(not_body1_layer, GColorBlack);
	text_layer_set_text_alignment(not_body1_layer, GTextAlignmentCenter); 
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(not_body1_layer));

	text_layer_set_background_color(not_body2_layer, GColorClear);
	text_layer_set_text_color(not_body2_layer, GColorBlack);
	text_layer_set_text_alignment(not_body2_layer, GTextAlignmentCenter); 
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(not_body2_layer));

	text_layer_set_background_color(not_body3_layer, GColorClear);
	text_layer_set_text_color(not_body3_layer, GColorBlack);
	text_layer_set_text_alignment(not_body3_layer, GTextAlignmentCenter); 
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(not_body3_layer));


    }
}

// when the secondary window loads, know which screen to load

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

// click handler to cancel alert sending

void fall_select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
    if (did_fall != 1) return;
    did_fall = 0;
    text_layer_set_text(fall_body_layer, "ALERT SENDING TO CARETAKER HAS BEEN CANCELED. PRESS BACK TO GO BACK TO APP MENU");
}

// logic to handle dead man switch

void sel_handler(ClickRecognizerRef recognizer, void *context) {


    if(row != 1) return;

    if(in_dms_mode == 1){
	text_layer_set_text(dms_body_layer, "DEAD MAN SWITCH ACTIVATED. ALERT SENT TO PRIMARY CARETKER");
	send_alert();
	in_dms_mode = 2;
    }
    else if(in_dms_mode == 0){
	text_layer_set_text(dms_body_layer, "ENTERING DEAD MAN SWITCH MODE. PRESS DOWN BUTTON TO CANCEL.");
	in_dms_mode = 1;
    }
}

// left the dead man switch mode

void sel_handler2(ClickRecognizerRef recognizer, void *context) {
    if(in_dms_mode != 1) return;
    text_layer_set_text(dms_body_layer, "LEFT DEAD MAN SWITCH MODE.");
    in_dms_mode = 2;
}

void config_provider(Window *window) {
    window_single_repeating_click_subscribe(BUTTON_ID_SELECT, 1000, fall_select_single_click_handler);
}


void config_provider2(Window *window) {
    window_raw_click_subscribe(BUTTON_ID_UP, sel_handler, sel_handler, NULL);
    window_raw_click_subscribe(BUTTON_ID_DOWN, sel_handler2, sel_handler2, NULL);
}

// window load when we detect a fall

void fall_window_load(Window *window){


    push_msg(nots[4]);
    is_fall_screen = 1;
    did_fall = 1;

    fall_text_layer = text_layer_create(GRect(0, 0, 144, 168));
    text_layer_set_font(fall_text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));		
    text_layer_set_background_color(fall_text_layer, GColorClear);
    text_layer_set_text_color(fall_text_layer, GColorBlack);
    text_layer_set_text_alignment(fall_text_layer, GTextAlignmentCenter); 
    text_layer_set_text(fall_text_layer, "FALL DETECTED");


    text_layer_set_background_color(fall_body_layer, GColorClear);
    text_layer_set_text_color(fall_body_layer, GColorBlack);
    text_layer_set_text_alignment(fall_body_layer, GTextAlignmentCenter); 


    static const uint32_t const segments[] = { 200, 100, 200, 100, 200 };
    VibePattern pat = {
	.durations = segments,
	.num_segments = ARRAY_LENGTH(segments),
    };
    vibes_enqueue_custom_pattern(pat);

    layer_add_child(window_get_root_layer(window), text_layer_get_layer(fall_text_layer));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(fall_body_layer));
}

void window_unload(Window *window){

    text_layer_destroy(not_body1_layer);
    text_layer_destroy(not_body2_layer);
    text_layer_destroy(not_body3_layer);
    text_layer_destroy(help_body_layer);
    text_layer_destroy(dms_body_layer);

    text_layer_destroy(fall_body_layer);
    text_layer_destroy(text_time_layer);
    text_layer_destroy(text_date_layer);
    window_destroy(sec_window);
    menu_layer_destroy(menu_layer);
}

void sec_window_unload(Window *window){
    text_layer_destroy(sec_text_layer);
}

void fall_window_unload(Window *window){
    is_fall_screen = 0;
    did_fall = 0;
    text_layer_destroy(fall_text_layer);
}

// just callsback that are called when a message passes through the inbox/outbox

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    // Store incoming information
    int sent_alert=1;
    int sent_loc=1;


    // Read first item
    Tuple *t = dict_read_first(iterator);

    // For all items
    while(t != NULL) {
	// Which key was received?
	switch(t->key) {
	    // Temperature, update temp buffer.
	    case ALERT_RESULT:
		sent_alert = (int)t->value->int32;
		break;
		// Weather, update weather buffer.

	    case LOC_RESULT:
		sent_loc = (int)t->value->int32;
		break;

	    default:
		APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
		break;
	}

	// Look for next item
	t = dict_read_next(iterator);
    }

    if(sent_alert == 0) { push_msg(nots[0]);}
    if(sent_loc == 0) { push_msg(nots[1]);}

    if(sent_alert == 1) { push_msg(nots[2]);}
    if(sent_loc == 1) { push_msg(nots[3]);}
}

// Callbacks when message is sent or not sent from the message box.

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}


static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {

    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

// init when the app starts

void init()
{

    not_pointer = 0;
    is_fall_screen = 0;
    in_dms_mode = 0;
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

    fall_window = window_create();
    WindowHandlers fall_handlers = {
	.load = fall_window_load,
	.unload = fall_window_unload
    };

    window_set_window_handlers(fall_window, (WindowHandlers) fall_handlers);
    window_set_window_handlers(sec_window, (WindowHandlers) sec_handlers);

    window_set_click_config_provider(fall_window, (ClickConfigProvider) config_provider);
    window_set_click_config_provider(sec_window, (ClickConfigProvider) config_provider2);
    // Register with TickTimerService
    tick_timer_service_subscribe(SECOND_UNIT, handle_minute_tick);
    handle_minute_tick(NULL, SECOND_UNIT);

    app_worker_launch();
    // Subscribe to Worker messages
    app_worker_message_subscribe(worker_message_handler); 

    // Register callbacks
    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);

    // Open AppMessage
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());


}

void deinit()
{
    window_destroy(window);
    tick_timer_service_unsubscribe();
    // No more worker updates
    app_worker_message_unsubscribe();
}

int main(void){
    init();
    app_event_loop();
    deinit();

    return 0;
}

