package main

import (
	"flag"
	"fmt"
)

var verbose = flag.Bool("v", false, "enable verbose output")

func main() {
	// parse flags
	flag.Parse()

	// handle commands
	switch flag.Arg(0) {
	case "bundle":
		bundle(flag.Arg(1), flag.Arg(2))
	case "upload":
		upload(flag.Arg(1), flag.Arg(2))
	case "launch":
		launch(flag.Arg(1))
	default:
		fmt.Println("unknown command: ", flag.Arg(0))
	}
}
