package main

import (
	"fmt"
	"os"
	"path/filepath"
	"reflect"
	"strings"

	"github.com/samber/lo"
	"gopkg.in/yaml.v3"

	"tools/alp"
)

func bundle(dir, out string) {
	// determine root
	root, err := filepath.Abs(dir)
	if err != nil {
		panic(err)
	}

	// load manifest
	manifestRaw, err := os.ReadFile(filepath.Join(root, "alp.yml"))
	if err != nil {
		panic(err)
	}

	// parse manifest
	var manifest alp.Manifest
	err = yaml.Unmarshal(manifestRaw, &manifest)
	if err != nil {
		panic(err)
	}

	// resolves sprites
	var sprites []string
	for _, sprite := range manifest.Sprites {
		matches, err := filepath.Glob(filepath.Join(root, sprite))
		if err != nil {
			panic(err)
		}
		sprites = append(sprites, matches...)
	}

	// print info
	fmt.Printf("Name: %s\n", manifest.Name)
	fmt.Printf("Binary: %s\n", manifest.Binary)

	// print sprites
	if len(sprites) > 0 {
		fmt.Printf("Sprites:\n")
		for _, sprite := range sprites {
			fmt.Printf(" - %s\n", sprite)
		}
	}

	/* create bundle */

	// prepare bundle
	var bundle alp.Bundle

	// add name attribute
	bundle.Sections = append(bundle.Sections, alp.BundleSection{
		Type: alp.BundleTypeAttr,
		Name: "name",
		Data: []byte(manifest.Name),
	})

	// add binary
	binPath := filepath.Join(root, manifest.Binary)
	binData, err := os.ReadFile(binPath)
	if err != nil {
		panic(err)
	}
	bundle.Sections = append(bundle.Sections, alp.BundleSection{
		Type: alp.BundleTypeBinary,
		Name: "main",
		Data: binData,
	})

	// add sprites
	for _, sprite := range sprites {
		spriteData, err := os.ReadFile(sprite)
		if err != nil {
			panic(err)
		}
		if filepath.Ext(sprite) == ".png" {
			spriteData = alp.SpriteFromPNG(spriteData, 1).Encode()
			sprite = strings.TrimSuffix(sprite, ".png")
		}
		bundle.Sections = append(bundle.Sections, alp.BundleSection{
			Type: alp.BundleTypeSprite,
			Name: lo.Must(filepath.Rel(root, sprite)),
			Data: spriteData,
		})
	}

	// determine output file
	file, err := filepath.Abs(out)
	if err != nil {
		panic(err)
	}

	// create file
	err = os.WriteFile(file, bundle.Encode(), 0644)
	if err != nil {
		panic(err)
	}

	/* verify bundle */

	// read file
	data, err := os.ReadFile(file)
	if err != nil {
		panic(err)
	}

	// decode bundle
	var verify alp.Bundle
	err = verify.Decode(data)
	if err != nil {
		panic(err)
	}

	// print bundle info
	fmt.Printf("Wrote: %s\n", file)
	if *verbose {
		fmt.Printf("Sections: %d\n", len(verify.Sections))
		for i, section := range verify.Sections {
			fmt.Printf(" - Section %d: Type=%d Name=%q Size=%d\n", i, section.Type, section.Name, len(section.Data))
		}
	}

	// compare bundles
	if !reflect.DeepEqual(bundle, verify) {
		panic("bundle verification failed")
	}
}
