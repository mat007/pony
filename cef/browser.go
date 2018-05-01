package browser

import (
	"unsafe"
)

/*
#include <stdlib.h>

void start_browser(const char* url, int log);

typedef struct go_string_t {
  char* str;
  int length;
} go_string;

#cgo LDFLAGS: -LRelease -lcef
*/
import "C"

// Logger receives console.log calls from the Javascript code.
type Logger func(level int, message, source string, line int)

// Start creates a browser window pointing to the given url, then blocks until the window is closed.
// This function must be called only once per application.
func Start(url string, logger Logger) {
	u := C.CString(url)
	defer C.free(unsafe.Pointer(u))
	C.start_browser(u, setLog(logger))
}

var l Logger

//export goLog
func goLog(level int, message *C.go_string, source *C.go_string, line int) {
	if l != nil {
		l(level, toString(message), toString(source), line)
	}
}

func toString(s *C.go_string) string {
	return C.GoStringN(s.str, s.length)
}

func setLog(logger Logger) C.int {
	if logger == nil {
		return 0
	}
	l = logger
	return 1
}
