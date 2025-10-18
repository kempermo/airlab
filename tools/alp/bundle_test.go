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
	assert.Equal(t, []byte{
		'A', 'L', 'P', 0x00, // magic + version
		0x0, 0x0, 0x0, 0x26, // header length
		0x0, 0x2, // section count
		// section 1
		0x0,                 // type
		0x0, 0x0, 0x0, 0x26, // offset
		0x0, 0x0, 0x0, 0x7, // length
		'n', 'a', 'm', 'e', 0x0, // name + null
		// section 2
		0x1,                 // type
		0x0, 0x0, 0x0, 0x2d, // offset
		0x0, 0x0, 0x0, 0x4, // length
		'm', 'a', 'i', 'n', 0x0, // name + null
		// section Data
		'T', 'e', 's', 't', 'A', 'p', 'p', // section 1
		0x1, 0x2, 0x3, 0x4, // section 2
	}, out)

	b2, err := DecodeBundle(out)
	assert.NoError(t, err)
	assert.Equal(t, b, *b2)
}
