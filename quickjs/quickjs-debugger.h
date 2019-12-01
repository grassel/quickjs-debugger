#ifndef QUICKJS_DEBUGGER_H
#define QUICKJS_DEBUGGER_H

#include "quickjs.h"
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct JSDebuggerFunctionInfo {
    // same length as byte_code_buf.
    uint8_t *breakpoints;
    uint32_t dirty;
    int last_line_num;
} JSDebuggerFunctionInfo;

typedef struct JSDebuggerLocation {
    JSAtom filename;
    int line;
    int column;
} JSDebuggerLocation;

#define JS_DEBUGGER_STEP 1
#define JS_DEBUGGER_STEP_IN 2
#define JS_DEBUGGER_STEP_OUT 3
#define JS_DEBUGGER_STEP_CONTINUE 4

typedef struct JSDebuggerInfo {
    JSContext *ctx;
 
    int attempted_connect;
    int attempted_wait;
    int peek_ticks;
    int should_peek;
    char *message_buffer;
    int message_buffer_length;
    int is_debugging;
    int is_paused;

    size_t (*transport_read)(void *udata, char* buffer, size_t length);
    size_t (*transport_write)(void *udata, const char* buffer, size_t length);
    size_t (*transport_peek)(void *udata);
    void (*transport_close)(JSContext *ctx, void *udata);
    void *transport_udata;

    JSValue breakpoints;
    int exception_breakpoint;
    uint32_t breakpoints_dirty_counter;
    int stepping;
    JSDebuggerLocation step_over;
    int step_depth;
} JSDebuggerInfo;

void js_debugger_check(JSContext *ctx, const uint8_t *pc);
void js_debugger_exception(JSContext* ctx);
void js_debugger_free(JSContext *ctx, JSDebuggerInfo *info);

void js_debugger_attach(
    JSContext* ctx,
    size_t (*transport_read)(void *udata, char* buffer, size_t length),
    size_t (*transport_write)(void *udata, const char* buffer, size_t length),
    size_t (*transport_peek)(void *udata),
    void (*transport_close)(JSContext *ctx, void *udata),
    void *udata
);
void js_debugger_connect(JSContext *ctx, const char *address);
void js_debugger_wait_connection(JSContext *ctx, const char* address);
int js_debugger_is_transport_connected(JSContext *ctx);

JSValue js_debugger_file_breakpoints(JSContext *ctx, const char *path);
void js_debugger_cooperate(JSContext *ctx);

// begin internal api functions
// these functions all require access to quickjs internal structures.

JSDebuggerInfo *js_debugger_info(JSContext *ctx);

// this may be able to be done with an Error backtrace,
// but would be clunky and require stack string parsing.
uint32_t js_debugger_stack_depth(JSContext *ctx);
JSValue js_debugger_build_backtrace(JSContext *ctx, const uint8_t *cur_pc);
JSDebuggerLocation js_debugger_current_location(JSContext *ctx, const uint8_t *cur_pc);

// checks to see if a breakpoint exists on the current pc.
// calls back into js_debugger_file_breakpoints.
int js_debugger_check_breakpoint(JSContext *ctx, uint32_t current_dirty, const uint8_t *cur_pc);

// could be moved out if js_json_stringify is made public.
// could also be implemented by constructing an eval call. clunky.
JSValue js_debugger_json_stringify(JSContext *ctx, JSValueConst value);

JSValue js_debugger_local_variables(JSContext *ctx, int stack_index);
JSValue js_debugger_closure_variables(JSContext *ctx, int stack_index);

// evaluates an expression at any stack frame. JS_Evaluate* only evaluates at the top frame.
JSValue js_debugger_evaluate(JSContext *ctx, int stack_index, JSValue expression);

// end internal api functions

#ifdef __cplusplus
}
#endif

#endif
