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

	// create session
	sess, err := man.NewSession()
	if err != nil {
		panic(err)
	}
	defer sess.End(0)

	// kill running plugin
	err = sess.Send(0xA1, []byte{0x3}, 5*time.Second)
	if err != nil {
		panic(err)
	}

	// launch plugin
	err = sess.Send(0xA1, append([]byte{0x2}, []byte(name)...), 5*time.Second)
	if err != nil {
		panic(err)
	}

	// log
	fmt.Printf("==> Launched!\n")

	// start log streaming
	err = sess.Send(0xA1, []byte{0x4}, 5*time.Second)
	if err != nil {
		panic(err)
	}

	// TODO: Prevent session timeout.

	// receive logs
	for {
		log, err := sess.Receive(0xA1, false, time.Minute)
		if err != nil {
			panic(err)
		}
		fmt.Printf("==> Log: %s\n", string(log))
	}
}
