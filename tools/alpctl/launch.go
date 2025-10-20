package main

import (
	"fmt"
	"time"

	"github.com/256dpi/naos/pkg/msg"
	"github.com/256dpi/naos/pkg/serial"
)

func launch(name, device string) {
	// check name
	if name == "" {
		panic("missing name")
	}

	// open device
	var dev msg.Device
	var err error
	if device != "" {
		dev, err = serial.Open(device)
	} else {
		dev, err = serial.OpenBest()
	}
	if err != nil {
		panic(err)
	}

	// log
	fmt.Printf("==> Found: %s\n", dev.ID())

	// prepare device
	man := msg.NewManagedDevice(dev)
	defer man.Deactivate()
	err = man.Activate()
	if err != nil {
		panic(err)
	}

	// launch plugin
	err = man.UseSession(func(s *msg.Session) error {
		return s.Send(0xA1, append([]byte{0x2}, []byte(name)...), time.Second)
	})
	if err != nil {
		panic(err)
	}

	// log
	fmt.Printf("==> Launched!\n")
}
