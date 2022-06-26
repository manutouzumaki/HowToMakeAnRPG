#ifndef TEXTURE_H
#define TEXTURE_H

#pragma pack(push, 1)
struct bmp_header
{
    u16 FileType;
    u32 FileSize;
    u16 Reserved1;
    u16 Reserved2;
    u32 BitmapOffset;
	u32 Size;             
	i32 Width;            
    i32 Height;           
	u16 Planes;           
	u16 BitsPerPixel;    
	u32 Compression;      
	u32 SizeOfBitmap;     
	i32 HorzResolution;  
	i32 VertResolution;  
	u32 ColorsUsed;       
	u32 ColorsImportant;  
	u32 RedMask;          
	u32 GreenMask;        
	u32 BlueMask;         
	u32 AlphaMask;        
};
#pragma pack(pop)

struct texture
{
    u32 ID;
    i32 Width;
    i32 Height;
};


#endif
