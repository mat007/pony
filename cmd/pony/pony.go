package main

import (
	"flag"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"strings"
)

var verbose = flag.Bool("v", false, "increase verbosity")

func main() {
	flag.Usage = func() {
		fmt.Fprintf(os.Stderr, `Usage: %s [options] install <destination folder>

Binaries and resources installation tool

Options:
`, os.Args[0])
		flag.PrintDefaults()
		os.Exit(1)
	}
	flag.Parse()
	if flag.NArg() != 2 || flag.Arg(0) != "install" {
		flag.Usage()
	}
	gopath := os.Getenv("GOPATH")
	paths := strings.Split(gopath, string(os.PathListSeparator))
	for i := range paths {
		paths[i] = filepath.Join(paths[i], "src")
	}
	paths = append([]string{"vendor"}, paths...)
	for _, path := range paths {
		from := filepath.Join(path, "github.com/mat007/pony")
		if err := install(from, flag.Arg(1)); err == nil {
			return
		} else if !os.IsNotExist(err) {
			fmt.Println(err.Error())
			os.Exit(1)
		}
	}
	fmt.Println("cannot find data under GOPATH:", gopath)
	os.Exit(1)
}

func install(from, to string) error {
	if *verbose {
		fmt.Println("looking for data in", from)
	}
	if _, err := os.Stat(from); err != nil {
		return err
	}
	if *verbose {
		fmt.Println("found data in", from)
	}
	if err := os.MkdirAll(to, os.ModePerm); err != nil {
		return err
	}
	if err := walk(filepath.Join(from, "cef", "Release"), to,
		func(path string) bool {
			return filepath.Ext(path) != ".lib"
		}); err != nil {
		return err
	}
	return walk(filepath.Join(from, "cef", "Resources"), to, nil)
}

func walk(from, to string, filter func(path string) bool) error {
	return filepath.Walk(from,
		func(path string, info os.FileInfo, err error) error {
			if err != nil || filter != nil && !filter(path) || info.IsDir() {
				return nil
			}
			rel, err := filepath.Rel(from, path)
			if err != nil {
				return err
			}
			return copy(path, filepath.Join(to, rel))
		})
}

func copy(from, to string) error {
	if *verbose {
		fmt.Println("copying", from, "->", to)
	}
	if err := os.MkdirAll(filepath.Dir(to), os.ModePerm); err != nil {
		return err
	}
	reader, err := os.Open(from)
	if err != nil {
		return err
	}
	defer reader.Close()
	writer, err := os.Create(to)
	if err != nil {
		return err
	}
	defer writer.Close()
	if _, err := io.Copy(writer, reader); err != nil {
		return err
	}
	// TODO keep file timestamp?
	return nil
}
