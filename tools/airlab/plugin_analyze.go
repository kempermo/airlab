package main

import (
	"fmt"
	"os"

	"github.com/spf13/cobra"

	"github.com/networkedartifacts/airlab/tools/alb"
)

var pluginAnalyzeCmd = &cobra.Command{
	Use:   "analyze <file>",
	Short: "Analyze a plugin bundle file",
	Args:  cobra.ExactArgs(1),
	RunE: func(cmd *cobra.Command, args []string) error {
		return pluginAnalyze(args[0])
	},
}

func init() {
	pluginCmd.AddCommand(pluginAnalyzeCmd)
}

func pluginAnalyze(file string) error {
	// read file
	data, err := os.ReadFile(file)
	if err != nil {
		return err
	}

	// decode bundle
	bundle, err := alb.DecodeBundle(data)
	if err != nil {
		return err
	}

	// print magic
	fmt.Printf("Format: %q\n", string(data[0:4]))

	// print sections
	fmt.Printf("Sections: (%d total, %d bytes)\n", len(bundle.Sections), len(data))
	for _, sec := range bundle.Sections {
		fmt.Printf(" - [%s] %-20s %d bytes (flags: 0x%04x)\n", sec.Type, sec.Name, len(sec.Data), sec.Flags)
		switch {
		case sec.Type == alb.BundleTypeAttr:
			fmt.Printf("       %s\n", string(sec.Data))
		case sec.Type == alb.BundleTypeConfig:
			sub, err := alb.DecodeBundle(sec.Data)
			if err != nil {
				fmt.Printf("       (failed to decode: %v)\n", err)
				continue
			}
			config, err := alb.DecodeConfig(sub)
			if err != nil {
				fmt.Printf("       (failed to decode: %v)\n", err)
				continue
			}
			for _, section := range config.Sections {
				fmt.Printf("       - %s\n", section.Title)
				for _, item := range section.Items {
					fmt.Printf("         - %s (%s)\n", item.Key, item.Type)
				}
			}
		}
	}

	return nil
}
