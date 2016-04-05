#define PATCHES_PER_SCANLINE 12

const uint8_t TextureScaleLUT[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 
	0x38, 0x01, 0x48, 0x02, 0x49, 0x02, 0x65, 0x03, 0x66, 0x03, 0xa8, 0x04, 0xa9, 0x04, 0xaa, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x47, 0x02, 0x64, 0x03, 0xa6, 0x04, 0xa7, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x37, 0x01, 0x46, 0x02, 0x62, 0x03, 0x63, 0x03, 0xa3, 0x04, 0xa4, 0x04, 0xa5, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x36, 0x01, 0x45, 0x02, 0x60, 0x03, 0x61, 0x03, 0xa0, 0x04, 0xa1, 0x04, 0xa2, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x35, 0x01, 0x44, 0x02, 0x5f, 0x03, 0x9e, 0x04, 0x9f, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x34, 0x01, 0x43, 0x02, 0x5d, 0x03, 0x5e, 0x03, 0x9b, 0x04, 0x9c, 0x04, 0x9d, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x33, 0x01, 0x42, 0x02, 0x5c, 0x03, 0x98, 0x04, 0x99, 0x04, 0x9a, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x32, 0x01, 0x40, 0x02, 0x41, 0x02, 0x5a, 0x03, 0x5b, 0x03, 0x96, 0x04, 0x97, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x31, 0x01, 0x3f, 0x02, 0x58, 0x03, 0x59, 0x03, 0x93, 0x04, 0x94, 0x04, 0x95, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x30, 0x01, 0x3e, 0x02, 0x57, 0x03, 0x90, 0x04, 0x91, 0x04, 0x92, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x3d, 0x02, 0x55, 0x03, 0x56, 0x03, 0x8e, 0x04, 0x8f, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x2f, 0x01, 0x3c, 0x02, 0x54, 0x03, 0x8b, 0x04, 0x8c, 0x04, 0x8d, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x2e, 0x01, 0x3b, 0x02, 0x52, 0x03, 0x53, 0x03, 0x88, 0x04, 0x89, 0x04, 0x8a, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x2d, 0x01, 0x3a, 0x02, 0x50, 0x03, 0x51, 0x03, 0x86, 0x04, 0x87, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x2c, 0x01, 0x38, 0x02, 0x39, 0x02, 0x4f, 0x03, 0x83, 0x04, 0x84, 0x04, 0x85, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x2b, 0x01, 0x37, 0x02, 0x4d, 0x03, 0x4e, 0x03, 0x80, 0x04, 0x81, 0x04, 0x82, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x2a, 0x01, 0x36, 0x02, 0x4c, 0x03, 0x7e, 0x04, 0x7f, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x29, 0x01, 0x35, 0x02, 0x4a, 0x03, 0x4b, 0x03, 0x7b, 0x04, 0x7c, 0x04, 0x7d, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x28, 0x01, 0x34, 0x02, 0x48, 0x03, 0x49, 0x03, 0x78, 0x04, 0x79, 0x04, 0x7a, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x33, 0x02, 0x47, 0x03, 0x76, 0x04, 0x77, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x27, 0x01, 0x32, 0x02, 0x45, 0x03, 0x46, 0x03, 0x73, 0x04, 0x74, 0x04, 0x75, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x26, 0x01, 0x30, 0x02, 0x31, 0x02, 0x44, 0x03, 0x70, 0x04, 0x71, 0x04, 0x72, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x25, 0x01, 0x2f, 0x02, 0x42, 0x03, 0x43, 0x03, 0x6e, 0x04, 0x6f, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x24, 0x01, 0x2e, 0x02, 0x40, 0x03, 0x41, 0x03, 0x6b, 0x04, 0x6c, 0x04, 0x6d, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x23, 0x01, 0x2d, 0x02, 0x3f, 0x03, 0x68, 0x04, 0x69, 0x04, 0x6a, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x22, 0x01, 0x2c, 0x02, 0x3d, 0x03, 0x3e, 0x03, 0x66, 0x04, 0x67, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x21, 0x01, 0x2b, 0x02, 0x3c, 0x03, 0x63, 0x04, 0x64, 0x04, 0x65, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x20, 0x01, 0x2a, 0x02, 0x3a, 0x03, 0x3b, 0x03, 0x60, 0x04, 0x61, 0x04, 0x62, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x28, 0x02, 0x29, 0x02, 0x38, 0x03, 0x39, 0x03, 0x5e, 0x04, 0x5f, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x1f, 0x01, 0x27, 0x02, 0x37, 0x03, 0x5b, 0x04, 0x5c, 0x04, 0x5d, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x1e, 0x01, 0x26, 0x02, 0x35, 0x03, 0x36, 0x03, 0x58, 0x04, 0x59, 0x04, 0x5a, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x1d, 0x01, 0x25, 0x02, 0x34, 0x03, 0x56, 0x04, 0x57, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x1c, 0x01, 0x24, 0x02, 0x32, 0x03, 0x33, 0x03, 0x53, 0x04, 0x54, 0x04, 0x55, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x1b, 0x01, 0x23, 0x02, 0x30, 0x03, 0x31, 0x03, 0x50, 0x04, 0x51, 0x04, 0x52, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x1a, 0x01, 0x22, 0x02, 0x2f, 0x03, 0x4e, 0x04, 0x4f, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x19, 0x01, 0x20, 0x02, 0x21, 0x02, 0x2d, 0x03, 0x2e, 0x03, 0x4b, 0x04, 0x4c, 0x04, 0x4d, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x18, 0x01, 0x1f, 0x02, 0x2c, 0x03, 0x48, 0x04, 0x49, 0x04, 0x4a, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x1e, 0x02, 0x2a, 0x03, 0x2b, 0x03, 0x46, 0x04, 0x47, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x17, 0x01, 0x1d, 0x02, 0x28, 0x03, 0x29, 0x03, 0x43, 0x04, 0x44, 0x04, 0x45, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x16, 0x01, 0x1c, 0x02, 0x27, 0x03, 0x40, 0x04, 0x41, 0x04, 0x42, 0x04, 0xc0, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x15, 0x01, 0x1b, 0x02, 0x25, 0x03, 0x26, 0x03, 0x3e, 0x04, 0x3f, 0x04, 0xb8, 0x05, 0xb9, 0x05, 0xba, 0x05, 0xbb, 0x05, 0xbc, 0x05, 0xbd, 0x05, 
	0x14, 0x01, 0xbe, 0x05, 0xbf, 0x05, 0x1a, 0x02, 0x24, 0x03, 0x3b, 0x04, 0x3c, 0x04, 0x3d, 0x04, 0xb0, 0x05, 0xb1, 0x05, 0xb2, 0x05, 0xb3, 0x05, 
	0x13, 0x01, 0xb4, 0x05, 0xb5, 0x05, 0xb6, 0x05, 0xb7, 0x05, 0x18, 0x02, 0x19, 0x02, 0x22, 0x03, 0x23, 0x03, 0x38, 0x04, 0x39, 0x04, 0x3a, 0x04, 
	0x12, 0x01, 0xa8, 0x05, 0xa9, 0x05, 0xaa, 0x05, 0xab, 0x05, 0xac, 0x05, 0xad, 0x05, 0xae, 0x05, 0xaf, 0x05, 0x17, 0x02, 0x20, 0x03, 0x21, 0x03, 
	0x11, 0x01, 0x36, 0x04, 0x37, 0x04, 0xa0, 0x05, 0xa1, 0x05, 0xa2, 0x05, 0xa3, 0x05, 0xa4, 0x05, 0xa5, 0x05, 0xa6, 0x05, 0xa7, 0x05, 0x16, 0x02, 
	0x10, 0x01, 0x1f, 0x03, 0x33, 0x04, 0x34, 0x04, 0x35, 0x04, 0x98, 0x05, 0x99, 0x05, 0x9a, 0x05, 0x9b, 0x05, 0x9c, 0x05, 0x9d, 0x05, 0x9e, 0x05, 
	0x9f, 0x05, 0x15, 0x02, 0x1d, 0x03, 0x1e, 0x03, 0x30, 0x04, 0x31, 0x04, 0x32, 0x04, 0x90, 0x05, 0x91, 0x05, 0x92, 0x05, 0x93, 0x05, 0x94, 0x05, 
	0x0f, 0x01, 0x95, 0x05, 0x96, 0x05, 0x97, 0x05, 0x14, 0x02, 0x1c, 0x03, 0x2e, 0x04, 0x2f, 0x04, 0x88, 0x05, 0x89, 0x05, 0x8a, 0x05, 0x8b, 0x05, 
	0x0e, 0x01, 0x8c, 0x05, 0x8d, 0x05, 0x8e, 0x05, 0x8f, 0x05, 0x13, 0x02, 0x1a, 0x03, 0x1b, 0x03, 0x2b, 0x04, 0x2c, 0x04, 0x2d, 0x04, 0x80, 0x05, 
	0x0d, 0x01, 0x81, 0x05, 0x82, 0x05, 0x83, 0x05, 0x84, 0x05, 0x85, 0x05, 0x86, 0x05, 0x87, 0x05, 0x12, 0x02, 0x18, 0x03, 0x19, 0x03, 0x28, 0x04, 
	0x0c, 0x01, 0x29, 0x04, 0x2a, 0x04, 0x78, 0x05, 0x79, 0x05, 0x7a, 0x05, 0x7b, 0x05, 0x7c, 0x05, 0x7d, 0x05, 0x7e, 0x05, 0x7f, 0x05, 0x10, 0x02, 
	0x0b, 0x01, 0x11, 0x02, 0x17, 0x03, 0x26, 0x04, 0x27, 0x04, 0x70, 0x05, 0x71, 0x05, 0x72, 0x05, 0x73, 0x05, 0x74, 0x05, 0x75, 0x05, 0x76, 0x05, 
	0x0a, 0x01, 0x77, 0x05, 0x0f, 0x02, 0x15, 0x03, 0x16, 0x03, 0x23, 0x04, 0x24, 0x04, 0x25, 0x04, 0x68, 0x05, 0x69, 0x05, 0x6a, 0x05, 0x6b, 0x05, 
	0x09, 0x01, 0x6c, 0x05, 0x6d, 0x05, 0x6e, 0x05, 0x6f, 0x05, 0x0e, 0x02, 0x14, 0x03, 0x20, 0x04, 0x21, 0x04, 0x22, 0x04, 0x60, 0x05, 0x61, 0x05, 
	0x08, 0x01, 0x62, 0x05, 0x63, 0x05, 0x64, 0x05, 0x65, 0x05, 0x66, 0x05, 0x67, 0x05, 0x0d, 0x02, 0x12, 0x03, 0x13, 0x03, 0x1e, 0x04, 0x1f, 0x04, 
	0x58, 0x05, 0x59, 0x05, 0x5a, 0x05, 0x5b, 0x05, 0x5c, 0x05, 0x5d, 0x05, 0x5e, 0x05, 0x5f, 0x05, 0x0c, 0x02, 0x10, 0x03, 0x11, 0x03, 0x1b, 0x04, 
	0x07, 0x01, 0x1c, 0x04, 0x1d, 0x04, 0x50, 0x05, 0x51, 0x05, 0x52, 0x05, 0x53, 0x05, 0x54, 0x05, 0x55, 0x05, 0x56, 0x05, 0x57, 0x05, 0x0b, 0x02, 
	0x06, 0x01, 0x0f, 0x03, 0x18, 0x04, 0x19, 0x04, 0x1a, 0x04, 0x48, 0x05, 0x49, 0x05, 0x4a, 0x05, 0x4b, 0x05, 0x4c, 0x05, 0x4d, 0x05, 0x4e, 0x05, 
	0x05, 0x01, 0x4f, 0x05, 0x0a, 0x03, 0x0d, 0x03, 0x0e, 0x04, 0x16, 0x04, 0x17, 0x04, 0x40, 0x05, 0x41, 0x05, 0x42, 0x05, 0x43, 0x05, 0x44, 0x05, 
	0x04, 0x01, 0x45, 0x05, 0x46, 0x05, 0x47, 0x05, 0x08, 0x03, 0x09, 0x03, 0x0c, 0x04, 0x13, 0x04, 0x14, 0x04, 0x15, 0x04, 0x38, 0x05, 0x39, 0x05, 
	0x03, 0x01, 0x3a, 0x05, 0x3b, 0x05, 0x3c, 0x05, 0x3d, 0x05, 0x3e, 0x05, 0x3f, 0x05, 0x07, 0x03, 0x0b, 0x04, 0x10, 0x04, 0x11, 0x04, 0x12, 0x04, 
	0x02, 0x01, 0x30, 0x05, 0x31, 0x05, 0x32, 0x05, 0x33, 0x05, 0x34, 0x05, 0x35, 0x05, 0x36, 0x05, 0x37, 0x05, 0x06, 0x04, 0x0f, 0x04, 0x28, 0x05, 
	0x01, 0x01, 0x29, 0x05, 0x2a, 0x05, 0x2b, 0x05, 0x2c, 0x05, 0x2d, 0x05, 0x2e, 0x05, 0x2f, 0x05, 0x05, 0x04, 0x0d, 0x05, 0x20, 0x05, 0x21, 0x05, 
	0x00, 0x01, 0x22, 0x05, 0x23, 0x05, 0x24, 0x05, 0x25, 0x05, 0x26, 0x05, 0x27, 0x05, 0x04, 0x05, 0x08, 0x05, 0x09, 0x05, 0x0a, 0x05, 0x18, 0x05, 
	0x01, 0x09, 0x00, 0x09, 0x19, 0x05, 0x1a, 0x05, 0x1b, 0x05, 0x1c, 0x05, 0x1d, 0x05, 0x1e, 0x05, 0x1f, 0x05, 0x03, 0x06, 0x07, 0x06, 0x10, 0x05, 
	0x02, 0x09, 0x11, 0x05, 0x12, 0x05, 0x13, 0x05, 0x14, 0x05, 0x15, 0x05, 0x16, 0x05, 0x17, 0x05, 0x0b, 0x06, 0x0c, 0x06, 0x0e, 0x06, 0x0f, 0x06, 
	0x03, 0x09, 0x05, 0x07, 0x06, 0x07, 0x04, 0x08, 0x08, 0x07, 0x09, 0x06, 0x0a, 0x06, 0x0d, 0x06, 0x10, 0x06, 0x07, 0x07, 0x11, 0x06, 0x12, 0x06, 
	0x04, 0x09, 0x13, 0x06, 0x14, 0x06, 0x15, 0x06, 0x16, 0x06, 0x17, 0x06, 0x18, 0x06, 0x05, 0x08, 0x06, 0x08, 0x09, 0x07, 0x0a, 0x07, 0x19, 0x06, 
	0x05, 0x09, 0x1a, 0x06, 0x1b, 0x06, 0x1c, 0x06, 0x1d, 0x06, 0x1e, 0x06, 0x1f, 0x06, 0x20, 0x06, 0x07, 0x08, 0x08, 0x08, 0x0b, 0x07, 0x0c, 0x07, 
	0x06, 0x09, 0x0d, 0x07, 0x21, 0x06, 0x22, 0x06, 0x23, 0x06, 0x24, 0x06, 0x25, 0x06, 0x26, 0x06, 0x27, 0x06, 0x28, 0x06, 0x09, 0x08, 0x0e, 0x07, 
	0x07, 0x09, 0x08, 0x09, 0x0f, 0x07, 0x10, 0x07, 0x29, 0x06, 0x2a, 0x06, 0x2b, 0x06, 0x2c, 0x06, 0x2d, 0x06, 0x2e, 0x06, 0x2f, 0x06, 0x30, 0x06, 
	0x09, 0x09, 0x0a, 0x08, 0x0b, 0x08, 0x11, 0x07, 0x12, 0x07, 0x31, 0x06, 0x32, 0x06, 0x33, 0x06, 0x34, 0x06, 0x35, 0x06, 0x36, 0x06, 0x37, 0x06, 
	0x0a, 0x09, 0x38, 0x06, 0x0c, 0x08, 0x13, 0x07, 0x14, 0x07, 0x15, 0x07, 0x39, 0x06, 0x3a, 0x06, 0x3b, 0x06, 0x3c, 0x06, 0x3d, 0x06, 0x3e, 0x06, 
	0x0b, 0x09, 0x3f, 0x06, 0x40, 0x06, 0x0d, 0x08, 0x0e, 0x08, 0x16, 0x07, 0x17, 0x07, 0x18, 0x07, 0x41, 0x06, 0x42, 0x06, 0x43, 0x06, 0x44, 0x06, 
	0x0c, 0x09, 0x45, 0x06, 0x46, 0x06, 0x47, 0x06, 0x48, 0x06, 0x0f, 0x08, 0x10, 0x08, 0x19, 0x07, 0x1a, 0x07, 0x49, 0x06, 0x4a, 0x06, 0x4b, 0x06, 
	0x0d, 0x09, 0x4c, 0x06, 0x4d, 0x06, 0x4e, 0x06, 0x4f, 0x06, 0x50, 0x06, 0x11, 0x08, 0x1b, 0x07, 0x1c, 0x07, 0x1d, 0x07, 0x51, 0x06, 0x52, 0x06, 
	0x0e, 0x09, 0x53, 0x06, 0x54, 0x06, 0x55, 0x06, 0x56, 0x06, 0x57, 0x06, 0x58, 0x06, 0x12, 0x08, 0x13, 0x08, 0x1e, 0x07, 0x1f, 0x07, 0x20, 0x07, 
	0x0f, 0x09, 0x10, 0x09, 0x59, 0x06, 0x5a, 0x06, 0x5b, 0x06, 0x5c, 0x06, 0x5d, 0x06, 0x5e, 0x06, 0x5f, 0x06, 0x60, 0x06, 0x14, 0x08, 0x21, 0x07, 
	0x11, 0x09, 0x22, 0x07, 0x61, 0x06, 0x62, 0x06, 0x63, 0x06, 0x64, 0x06, 0x65, 0x06, 0x66, 0x06, 0x67, 0x06, 0x68, 0x06, 0x15, 0x08, 0x16, 0x08, 
	0x12, 0x09, 0x23, 0x07, 0x24, 0x07, 0x25, 0x07, 0x69, 0x06, 0x6a, 0x06, 0x6b, 0x06, 0x6c, 0x06, 0x6d, 0x06, 0x6e, 0x06, 0x6f, 0x06, 0x70, 0x06, 
	0x13, 0x09, 0x17, 0x08, 0x18, 0x08, 0x26, 0x07, 0x27, 0x07, 0x28, 0x07, 0x71, 0x06, 0x72, 0x06, 0x73, 0x06, 0x74, 0x06, 0x75, 0x06, 0x76, 0x06, 
	0x14, 0x09, 0x77, 0x06, 0x78, 0x06, 0x19, 0x08, 0x29, 0x07, 0x2a, 0x07, 0x79, 0x06, 0x7a, 0x06, 0x7b, 0x06, 0x7c, 0x06, 0x7d, 0x06, 0x7e, 0x06, 
	0x15, 0x09, 0x7f, 0x06, 0x80, 0x06, 0x1a, 0x08, 0x1b, 0x08, 0x2b, 0x07, 0x2c, 0x07, 0x2d, 0x07, 0x81, 0x06, 0x82, 0x06, 0x83, 0x06, 0x84, 0x06, 
	0x16, 0x09, 0x85, 0x06, 0x86, 0x06, 0x87, 0x06, 0x88, 0x06, 0x1c, 0x08, 0x2e, 0x07, 0x2f, 0x07, 0x30, 0x07, 0x89, 0x06, 0x8a, 0x06, 0x8b, 0x06, 
	0x17, 0x09, 0x18, 0x09, 0x8c, 0x06, 0x8d, 0x06, 0x8e, 0x06, 0x8f, 0x06, 0x90, 0x06, 0x1d, 0x08, 0x1e, 0x08, 0x31, 0x07, 0x32, 0x07, 0x91, 0x06, 
	0x19, 0x09, 0x92, 0x06, 0x93, 0x06, 0x94, 0x06, 0x95, 0x06, 0x96, 0x06, 0x97, 0x06, 0x98, 0x06, 0x1f, 0x08, 0x20, 0x08, 0x33, 0x07, 0x34, 0x07, 
	0x1a, 0x09, 0x35, 0x07, 0x99, 0x06, 0x9a, 0x06, 0x9b, 0x06, 0x9c, 0x06, 0x9d, 0x06, 0x9e, 0x06, 0x9f, 0x06, 0xa0, 0x06, 0x21, 0x08, 0x36, 0x07, 
	0x1b, 0x09, 0x37, 0x07, 0x38, 0x07, 0xa1, 0x06, 0xa2, 0x06, 0xa3, 0x06, 0xa4, 0x06, 0xa5, 0x06, 0xa6, 0x06, 0xa7, 0x06, 0xa8, 0x06, 0x22, 0x08, 
	0x1c, 0x09, 0x23, 0x08, 0x39, 0x07, 0x3a, 0x07, 0xa9, 0x06, 0xaa, 0x06, 0xab, 0x06, 0xac, 0x06, 0xad, 0x06, 0xae, 0x06, 0xaf, 0x06, 0xb0, 0x06, 
	0x1d, 0x09, 0x24, 0x08, 0x3b, 0x07, 0x3c, 0x07, 0x3d, 0x07, 0xb1, 0x06, 0xb2, 0x06, 0xb3, 0x06, 0xb4, 0x06, 0xb5, 0x06, 0xb6, 0x06, 0xb7, 0x06, 
	0x1e, 0x09, 0xb8, 0x06, 0x25, 0x08, 0x26, 0x08, 0x3e, 0x07, 0x3f, 0x07, 0x40, 0x07, 0xb9, 0x06, 0xba, 0x06, 0xbb, 0x06, 0xbc, 0x06, 0xbd, 0x06, 
	0x1f, 0x09, 0x20, 0x09, 0xbe, 0x06, 0xbf, 0x06, 0xc0, 0x06, 0x27, 0x08, 0x28, 0x08, 0x41, 0x07, 0x42, 0x07, 0x29, 0x08, 0x43, 0x07, 0x44, 0x07, 
	0x21, 0x09, 0x45, 0x07, 0x2a, 0x08, 0x2b, 0x08, 0x46, 0x07, 0x47, 0x07, 0x48, 0x07, 0x2c, 0x08, 0x49, 0x07, 0x4a, 0x07, 0x2d, 0x08, 0x2e, 0x08, 
	0x22, 0x09, 0x4b, 0x07, 0x4c, 0x07, 0x4d, 0x07, 0x2f, 0x08, 0x30, 0x08, 0x4e, 0x07, 0x4f, 0x07, 0x50, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x23, 0x09, 0x31, 0x08, 0x51, 0x07, 0x52, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x24, 0x09, 0x32, 0x08, 0x33, 0x08, 0x53, 0x07, 0x54, 0x07, 0x55, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x25, 0x09, 0x34, 0x08, 0x56, 0x07, 0x57, 0x07, 0x58, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x26, 0x09, 0x35, 0x08, 0x36, 0x08, 0x59, 0x07, 0x5a, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x27, 0x09, 0x28, 0x09, 0x37, 0x08, 0x38, 0x08, 0x5b, 0x07, 0x5c, 0x07, 0x5d, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x29, 0x09, 0x39, 0x08, 0x5e, 0x07, 0x5f, 0x07, 0x60, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x2a, 0x09, 0x3a, 0x08, 0x3b, 0x08, 0x61, 0x07, 0x62, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x2b, 0x09, 0x3c, 0x08, 0x63, 0x07, 0x64, 0x07, 0x65, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x2c, 0x09, 0x3d, 0x08, 0x3e, 0x08, 0x66, 0x07, 0x67, 0x07, 0x68, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x2d, 0x09, 0x3f, 0x08, 0x40, 0x08, 0x69, 0x07, 0x6a, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x2e, 0x09, 0x41, 0x08, 0x6b, 0x07, 0x6c, 0x07, 0x6d, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x2f, 0x09, 0x30, 0x09, 0x42, 0x08, 0x43, 0x08, 0x6e, 0x07, 0x6f, 0x07, 0x70, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x31, 0x09, 0x44, 0x08, 0x71, 0x07, 0x72, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x32, 0x09, 0x45, 0x08, 0x46, 0x08, 0x73, 0x07, 0x74, 0x07, 0x75, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x33, 0x09, 0x47, 0x08, 0x48, 0x08, 0x76, 0x07, 0x77, 0x07, 0x78, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x34, 0x09, 0x49, 0x08, 0x79, 0x07, 0x7a, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x35, 0x09, 0x4a, 0x08, 0x4b, 0x08, 0x7b, 0x07, 0x7c, 0x07, 0x7d, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x36, 0x09, 0x4c, 0x08, 0x7e, 0x07, 0x7f, 0x07, 0x80, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x37, 0x09, 0x38, 0x09, 0x4d, 0x08, 0x4e, 0x08, 0x81, 0x07, 0x82, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x39, 0x09, 0x4f, 0x08, 0x50, 0x08, 0x83, 0x07, 0x84, 0x07, 0x85, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x3a, 0x09, 0x51, 0x08, 0x86, 0x07, 0x87, 0x07, 0x88, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x3b, 0x09, 0x52, 0x08, 0x53, 0x08, 0x89, 0x07, 0x8a, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x3c, 0x09, 0x54, 0x08, 0x8b, 0x07, 0x8c, 0x07, 0x8d, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x3d, 0x09, 0x55, 0x08, 0x56, 0x08, 0x8e, 0x07, 0x8f, 0x07, 0x90, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x3e, 0x09, 0x57, 0x08, 0x58, 0x08, 0x91, 0x07, 0x92, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x3f, 0x09, 0x40, 0x09, 0x59, 0x08, 0x93, 0x07, 0x94, 0x07, 0x95, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x41, 0x09, 0x5a, 0x08, 0x5b, 0x08, 0x96, 0x07, 0x97, 0x07, 0x98, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x42, 0x09, 0x5c, 0x08, 0x99, 0x07, 0x9a, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x43, 0x09, 0x5d, 0x08, 0x5e, 0x08, 0x9b, 0x07, 0x9c, 0x07, 0x9d, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x44, 0x09, 0x5f, 0x08, 0x60, 0x08, 0x9e, 0x07, 0x9f, 0x07, 0xa0, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x45, 0x09, 0x61, 0x08, 0xa1, 0x07, 0xa2, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x46, 0x09, 0x62, 0x08, 0x63, 0x08, 0xa3, 0x07, 0xa4, 0x07, 0xa5, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	0x47, 0x09, 0x48, 0x09, 0x64, 0x08, 0xa6, 0x07, 0xa7, 0x07, 0xa8, 0x07, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 
	
};