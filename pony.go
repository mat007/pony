package main

//go:generate pony install bin
import "github.com/mat007/pony/cef"

func main() {
	browser.Start("http://github.com/mat007/pony", nil)
}
