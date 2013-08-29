

#ifndef __JPEG_IMAGE_H__
#define __JPEG_IMAGE_H__

#include <math.h>

// Do not change, it doesn't work :)
#define LANCZOS_DEPTH 3

class JPEG_IMAGE
{
	public:
		enum JI_RESIZE_METHOD
		{
			RM_POINT_SAMPLE,
			RM_BILINEAR,
			RM_BOX,
			RM_LANCZOS3,

			RM_NONE,

			RM_FORCEDWORD		= 0xffffffff
		};

	public:
								JPEG_IMAGE();
								~JPEG_IMAGE();
				bool			ReadFromFile(char *pFileName);
				bool			SaveToFile(const char *pFileName, int nCompression);

				bool			CreateImage(unsigned int Width, unsigned int Heigth, unsigned int bpp);
				void			ClearImage(unsigned int ClearColor);
				void			BlitImageTo(JPEG_IMAGE &Dest, int BlitX, int BlitY);
		inline	unsigned int	Width(void)							{ return _SizeX; }
		inline	unsigned int	Height(void)						{ return _SizeY; }
		inline	unsigned int	Bpp(void)							{ return _BytesPerPixel; }
		inline	void			*GetData(void)						{ return _pData; }
		static	void			InitLanczosTable(void);

		float			AdjustForCorrectPixelRatio(unsigned int &nWidth, unsigned int &nHeight, bool bExpand=false);
		void			Resize(JI_RESIZE_METHOD Method, unsigned int Width, unsigned int Height);
		void			Rotate90(void);
		void			CropTo(unsigned int Width, unsigned int Height, unsigned int nOffsetX, unsigned int nOffsetY, JPEG_IMAGE &imgDest);

		void			ResizeMpix(float nDestMpix);

		void			ResizePointSample(unsigned int Width, unsigned int Height);
		void			ResizeBilinear(unsigned int Width, unsigned int Height);
		void			ResizeLanczos3(unsigned int Width, unsigned int Height);
		void			ResizeBox(unsigned int Width, unsigned int Height);

protected:

		//Lanczos
		static inline float	Lanczos3Table(float fValue)
		{
			int Idx = (int)((fValue+LANCZOS_DEPTH) * (65536/(LANCZOS_DEPTH*2)));
			if( Idx & 0xffff0000 )
				return 0.0f;
			return _pLanczos[Idx];
		}

		static inline float	sinc(float fValue)
		{
			if (fValue != 0.0f)
			{
				fValue = fValue * 3.14159265359f;
				return sinf(fValue) / fValue;
			}
			else
				return 1.0f;
		}

		static inline float Lanczos3(float fValue)
		{
			if (fValue < 0.0f)
				fValue = -fValue;
			if (fValue < LANCZOS_DEPTH)
				return sinc(fValue) * sinc(fValue / LANCZOS_DEPTH);
			else
				return 0.0f;
		}
	

	protected:
		static float	*_pLanczos;
		void			*_pData;
		int				_BytesPerPixel, _SizeX, _SizeY;
};



#endif
