#include "include/capi/cef_app_capi.h"
#include "include/capi/cef_browser_capi.h"
#include "include/capi/cef_client_capi.h"
#include "_cgo_export.h"

cef_life_span_handler_t g_life_span_handler = {0};
cef_display_handler_t g_display_handler = {0};
cef_context_menu_handler_t g_context_menu_handler = {0};

cef_life_span_handler_t* CEF_CALLBACK get_life_span_handler(cef_client_t* self) {
    return &g_life_span_handler;
}

cef_display_handler_t* CEF_CALLBACK get_display_handler(cef_client_t* self) {
    return &g_display_handler;
}

cef_context_menu_handler_t* CEF_CALLBACK get_context_menu_handler(cef_client_t* self) {
    return &g_context_menu_handler;
}

void set_icon(HWND hwnd, LPCWSTR filename, UINT flag, UINT icon) {
    SHFILEINFOW sfi = {0};
    SHGetFileInfoW(filename, 0, &sfi, sizeof(sfi), SHGFI_ICON|flag);
    if (sfi.hIcon)
        SendMessageW(hwnd, WM_SETICON, icon, (LPARAM)sfi.hIcon);
}

int browsers = 0;

void CEF_CALLBACK on_after_created(cef_life_span_handler_t* self, cef_browser_t* browser) {
    ++browsers;
    cef_browser_host_t* host = browser->get_host(browser);
    cef_window_handle_t hwnd = host->get_window_handle(host);
    WCHAR filename[MAX_PATH + 1];
    GetModuleFileNameW(NULL, filename, MAX_PATH + 1);
    set_icon(hwnd, filename, SHGFI_LARGEICON, ICON_BIG);
    set_icon(hwnd, filename, SHGFI_SMALLICON, ICON_SMALL);
}

void CEF_CALLBACK on_before_close(cef_life_span_handler_t* self, cef_browser_t* browser) {
    --browsers;
    if (!browsers)
        cef_quit_message_loop();
}

void CEF_CALLBACK on_title_change(cef_display_handler_t* self, cef_browser_t* browser, const cef_string_t* title) {
    cef_browser_host_t* host = browser->get_host(browser);
    cef_window_handle_t hwnd = host->get_window_handle(host);
    cef_string_wide_t ctitle = {0};
    cef_string_utf16_to_wide(title->str, title->length, &ctitle);
    SetWindowTextW(hwnd, ctitle.str);
    cef_string_wide_clear(&ctitle);
}

go_string* to_string(cef_string_utf8_t* s) {
    return (go_string*)s;
}

int CEF_CALLBACK on_console_message(cef_display_handler_t* self, cef_browser_t* browser, cef_log_severity_t level, const cef_string_t* message, const cef_string_t* source, int line) {
    cef_string_utf8_t cef_message = {0};
    cef_string_utf16_to_utf8(message->str, message->length, &cef_message);
    cef_string_utf8_t cef_source = {0};
    cef_string_utf16_to_utf8(source->str, source->length, &cef_source);
    goLog(level, to_string(&cef_message), to_string(&cef_source), line);
    cef_string_utf8_clear(&cef_message);
    cef_string_utf8_clear(&cef_source);
    return 1;
}

void CEF_CALLBACK on_before_context_menu(cef_context_menu_handler_t* self, cef_browser_t* browser, cef_frame_t* frame, cef_context_menu_params_t* params, cef_menu_model_t* model) {
    model->clear(model);
}

void start_browser(const char* url, int log) {
    cef_enable_highdpi_support();

	cef_main_args_t main_args = {0};
    main_args.instance = GetModuleHandle(NULL);

    cef_app_t app = {0};
    app.base.size = sizeof(cef_app_t);

    cef_settings_t settings = {0};
    settings.size = sizeof(cef_settings_t);
	settings.log_severity = LOGSEVERITY_WARNING;
    cef_string_t cef_subprocess_path = {0};
    cef_string_utf8_to_utf16("cefclient.exe", strlen("cefclient.exe"), &cef_subprocess_path);
	settings.browser_subprocess_path = cef_subprocess_path;
    settings.no_sandbox = 1;

    cef_initialize(&main_args, &settings, &app, NULL);

    cef_window_info_t window_info = {0};
    window_info.style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE;
    window_info.parent_window = NULL;
    window_info.x = CW_USEDEFAULT;
    window_info.y = CW_USEDEFAULT;
    window_info.width = CW_USEDEFAULT;
    window_info.height = CW_USEDEFAULT;

    cef_string_t cef_url = {0};
    cef_string_utf8_to_utf16(url, strlen(url), &cef_url);

    cef_browser_settings_t browser_settings = {0};
    browser_settings.size = sizeof(cef_browser_settings_t);

    cef_client_t client = {0};
    client.base.size = sizeof(cef_client_t);
    client.get_life_span_handler = get_life_span_handler;
    client.get_display_handler = get_display_handler;
    client.get_context_menu_handler = get_context_menu_handler;
    g_life_span_handler.base.size = sizeof(cef_life_span_handler_t);
    g_life_span_handler.on_before_close = on_before_close;
    g_life_span_handler.on_after_created = on_after_created;
    g_display_handler.base.size = sizeof(cef_display_handler_t);
    g_display_handler.on_title_change = on_title_change;
    if(log)
        g_display_handler.on_console_message = on_console_message;
    g_context_menu_handler.base.size = sizeof(cef_context_menu_handler_t);
    g_context_menu_handler.on_before_context_menu = on_before_context_menu;

    cef_browser_host_create_browser(&window_info, &client, &cef_url, &browser_settings, NULL);

    cef_run_message_loop();

    cef_shutdown();
}
