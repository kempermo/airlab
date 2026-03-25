package alp

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"

	"github.com/networkedartifacts/airlab/tools/alb"
)

// BuildBundle assembles an alb.Bundle from a manifest and its root directory.
func BuildBundle(manifest *Manifest, root string) (*alb.Bundle, error) {
	// resolve root
	root, err := filepath.Abs(root)
	if err != nil {
		return nil, err
	}

	// resolve sprites
	var sprites []string
	for _, sprite := range manifest.Sprites {
		matches, err := filepath.Glob(filepath.Join(root, sprite))
		if err != nil {
			return nil, err
		}
		sprites = append(sprites, matches...)
	}

	// prepare bundle
	var bundle alb.Bundle

	// add attributes
	bundle.AddAttr("name", []byte(manifest.Name))
	bundle.AddAttr("title", []byte(manifest.Title))
	bundle.AddAttr("version", []byte(manifest.Version))

	// add binaries
	for key, bin := range manifest.Binary {
		binPath := filepath.Join(root, bin)
		binData, err := os.ReadFile(binPath)
		if err != nil {
			return nil, err
		}
		bundle.Sections = append(bundle.Sections, alb.BundleSection{
			Type: alb.BundleTypeBinary,
			Name: key,
			Data: binData,
		})
	}

	// add sprites
	for _, sprite := range sprites {
		spriteData, err := os.ReadFile(sprite)
		if err != nil {
			return nil, err
		}
		if filepath.Ext(sprite) == ".png" {
			spriteData = alb.SpriteFromPNG(spriteData, 1).Encode()
			sprite = strings.TrimSuffix(sprite, ".png")
		}
		rel, err := filepath.Rel(root, sprite)
		if err != nil {
			return nil, err
		}
		bundle.Sections = append(bundle.Sections, alb.BundleSection{
			Type: alb.BundleTypeSprite,
			Name: rel,
			Data: spriteData,
		})
	}

	// add configs
	for key, cfg := range manifest.Config {
		if len(cfg.Sections) > 0 {
			configBundle, err := cfg.Encode()
			if err != nil {
				return nil, fmt.Errorf("config %q: %w", key, err)
			}
			bundle.Sections = append(bundle.Sections, alb.BundleSection{
				Type: alb.BundleTypeConfig,
				Name: key,
				Data: configBundle.Encode(),
			})
		}
	}

	return &bundle, nil
}
