package alp

import (
	"bytes"
	"fmt"
)

type Manifest struct {
	Name    string   `yaml:"name"`
	Binary  string   `yaml:"binary"`
	Sprites []string `yaml:"sprites"`
}

type BundleType byte

const (
	BundleTypeAttr   BundleType = 0x00
	BundleTypeBinary BundleType = 0x01
	BundleTypeSprite BundleType = 0x02
)

type BundleSection struct {
	Type BundleType
	Name string
	Data []byte
}

type Bundle struct {
	Sections []BundleSection
}

func (b *Bundle) Encode() []byte {
	// prepare buffer
	var out []byte

	// write header
	out = append(out, 'A', 'L', 'P', 0x00) // magic
	out = append(out, 0x01)                // version

	// number of section uint16
	numSections := len(b.Sections)
	out = append(out, byte(numSections>>8), byte(numSections)) // sections

	// write section types and sizes
	for _, section := range b.Sections {
		out = append(out, byte(section.Type))
		size := uint32(len(section.Data))
		out = append(out, byte(size>>24), byte(size>>16), byte(size>>8), byte(size))
		out = append(out, []byte(section.Name)...)
		out = append(out, 0)
	}

	// write sections
	for _, section := range b.Sections {
		out = append(out, section.Data...)
	}

	return out
}

func (b *Bundle) Decode(data []byte) error {
	// read header
	if len(data) < 7 {
		return fmt.Errorf("invalid bundle: too short")
	}

	// check magic
	if string(data[0:4]) != "ALP\x00" {
		return fmt.Errorf("invalid bundle: bad magic")
	}

	// check version
	if data[4] != 0x01 {
		return fmt.Errorf("invalid bundle: unsupported version")
	}

	// read sections
	sections := (int(data[5]) << 8) | int(data[6])
	offset := 7

	// read sections types and sizes
	var types []BundleType
	var sizes []uint32
	var names []string
	for i := 0; i < sections; i++ {
		types = append(types, BundleType(data[offset]))
		sizes = append(sizes, (uint32(data[offset+1])<<24)|(uint32(data[offset+2])<<16)|(uint32(data[offset+3])<<8)|uint32(data[offset+4]))
		offset += 5
		nameLen := bytes.IndexByte(data[offset:], 0)
		if offset+nameLen > len(data) {
			return fmt.Errorf("invalid bundle: section %d name too long", i)
		}
		offset += nameLen
		names = append(names, string(data[offset-nameLen:offset]))
		offset++
	}

	// read sections
	b.Sections = nil
	for i := 0; i < sections; i++ {
		if offset+int(sizes[i]) > len(data) {
			return fmt.Errorf("invalid bundle: section %d too large", i)
		}
		b.Sections = append(b.Sections, BundleSection{
			Type: types[i],
			Name: names[i],
			Data: data[offset : offset+int(sizes[i])],
		})
		offset += int(sizes[i])
	}

	return nil
}
