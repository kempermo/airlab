package main

import (
	"fmt"
	"os"
	"path/filepath"

	"github.com/spf13/cobra"

	"github.com/networkedartifacts/airlab/tools/alp"
)

var pluginBundleCmd = &cobra.Command{
	Use:   "bundle <dir> <output>",
	Short: "Bundle a plugin directory into a bundle file",
	Args:  cobra.ExactArgs(2),
	RunE: func(cmd *cobra.Command, args []string) error {
		return pluginBundle(args[0], args[1])
	},
}

func init() {
	pluginCmd.AddCommand(pluginBundleCmd)
}

func pluginBundle(dir, out string) error {
	// load manifest
	manifest, err := alp.LoadManifest(dir)
	if err != nil {
		return err
	}

	// print fields
	fmt.Printf("==> Name: %s\n", manifest.Name)
	fmt.Printf("==> Title: %s\n", manifest.Title)
	fmt.Printf("==> Version: %s\n", manifest.Version)

	// print binaries
	for key, bin := range manifest.Binary {
		fmt.Printf("==> Binary: %s (%s)\n", bin, key)
	}

	// print configs
	for key, cfg := range manifest.Config {
		if len(cfg.Sections) > 0 {
			fmt.Printf("Config (%s):\n", key)
			for _, section := range cfg.Sections {
				fmt.Printf(" - %s\n", section.Title)
				for _, item := range section.Items {
					fmt.Printf("   - %s (%s)\n", item.Key, item.Type)
				}
			}
		}
	}

	// build bundle
	bundle, err := alp.BuildBundle(manifest, dir)
	if err != nil {
		return err
	}

	// determine output file
	file, err := filepath.Abs(out)
	if err != nil {
		return err
	}

	// create file
	err = os.WriteFile(file, bundle.Encode(), 0644)
	if err != nil {
		return err
	}

	// print bundle info
	fmt.Printf("==> Wrote: %s\n", file)
	if pluginVerbose {
		fmt.Printf("==> Sections: %d\n", len(bundle.Sections))
		for i, section := range bundle.Sections {
			fmt.Printf("    Num=%d Type=%s Name=%q Size=%d\n", i, section.Type, section.Name, len(section.Data))
		}
	}

	return nil
}
