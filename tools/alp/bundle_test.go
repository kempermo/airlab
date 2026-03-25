package alp

import (
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestBundle(t *testing.T) {
	b := Bundle{
		Sections: []BundleSection{
			{
				Type: BundleTypeAttr,
				Name: "name",
				Data: []byte("TestApp"),
			},
			{
				Type: BundleTypeBinary,
				Name: "main",
				Data: []byte{0x1, 0x2, 0x3, 0x4},
			},
		},
	}

	out := b.Encode()

	// verify ALB encoding
	assert.Equal(t, []byte("ALB\x00"), out[0:4], "magic should be ALB")
	assert.Equal(t, enc.Uint32(out[4:8]), uint32(0x3a), "header length should include total size, flags, and header checksum")
	assert.Equal(t, enc.Uint32(out[10:14]), uint32(len(out)), "total size should match encoded length")

	b2, err := DecodeBundle(out)
	assert.NoError(t, err)
	assert.Equal(t, b, *b2)

	// verify legacy bundles are still decodable
	legacy := []byte{
		'A', 'L', 'P', 0x00, // magic
		0x2e, 0x0, 0x0, 0x0, // header length
		0x2, 0x0, // section count
		// section 1
		0x0,                 // type
		0x2e, 0x0, 0x0, 0x0, // offset
		0x7, 0x0, 0x0, 0x0, // length
		0xda, 0xce, 0x82, 0x46, // checksum
		'n', 'a', 'm', 'e', 0x0, // name + null
		// section 2
		0x1,                 // type
		0x36, 0x0, 0x0, 0x0, // offset
		0x4, 0x0, 0x0, 0x0, // length
		0xcd, 0xfb, 0x3c, 0xb6, // checksum
		'm', 'a', 'i', 'n', 0x0, // name + null
		// section data
		'T', 'e', 's', 't', 'A', 'p', 'p', 0x0, // section 1 + null
		0x1, 0x2, 0x3, 0x4, 0x0, // section 2 + null
	}
	b3, err := DecodeBundle(legacy)
	assert.NoError(t, err)
	assert.Equal(t, b, *b3)

	// verify modern header corruption is detected
	corrupted := make([]byte, len(out))
	copy(corrupted, out)
	corrupted[9] = 0xFF // corrupt section count byte
	_, err = DecodeBundle(corrupted)
	assert.Error(t, err)
}
