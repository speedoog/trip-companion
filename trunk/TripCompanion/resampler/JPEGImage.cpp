
#include "JPEGImage.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <math.h>
#include <vector>

#include "../jpeg/jpgd.h"
#include "../jpeg/jpge.h"
#include "resampler/resampler.h"


float	*JPEG_IMAGE::_pLanczos = 0;

typedef	unsigned char	UInt8;
typedef	unsigned int	UInt32;

//-------------------------------------------------------------------------------
JPEG_IMAGE::JPEG_IMAGE()
:
_pData(0),
_BytesPerPixel(0),
_SizeX(0),
_SizeY(0)
{
}

//-------------------------------------------------------------------------------
JPEG_IMAGE::~JPEG_IMAGE()
{
	if(_pData)
		free(_pData);
}

//---------------------------------------------------------------------------------------
void JPEG_IMAGE::Rotate90(void)
{
	UInt8 *pDest = (UInt8*)malloc( _SizeX * _SizeY * _BytesPerPixel );

	UInt32	nX;
	UInt32	nY;

	for(nX=0; nX<_SizeX; nX++)
	{
		for(nY=0; nY<_SizeY; nY++)
		{
			UInt32	nPixelBaseSrc	= ((_SizeX-1-nX) + nY*_SizeX)*3;
			UInt32	nPixelBaseDest	= (nY + nX*_SizeY)*3;

			pDest[nPixelBaseDest]	= ((UInt8*)_pData)[nPixelBaseSrc];
			pDest[nPixelBaseDest+1] = ((UInt8*)_pData)[nPixelBaseSrc+1];
			pDest[nPixelBaseDest+2] = ((UInt8*)_pData)[nPixelBaseSrc+2];
		}
	}

	free(_pData);
	_pData = pDest;
	_SizeX = nY;
	_SizeY = nX;
}

//---------------------------------------------------------------------------------------
void JPEG_IMAGE::BlitImageTo(JPEG_IMAGE &Dest, int BlitX, int BlitY)
{
	//if( Bpp() != Dest.Bpp() )
	//	return;

	unsigned char *pData	= (unsigned char*)Dest.GetData();
	unsigned char *pSource	= (unsigned char*)GetData();
	int		PixelSize		= Dest.Bpp();
	int		SrcPixelSize	= Bpp();

	if(!pData)
		return;

	int CurY;
	int CurX;

	for(CurY=BlitY; CurY<BlitY+(int)Height(); CurY++)
	{
		unsigned char *pPixel = pData;
		pPixel += (CurY*Dest.Width() + BlitX) * PixelSize;
		for(CurX=BlitX; CurX<BlitX+(int)Width(); CurX++)
		{
			switch(PixelSize)
			{
				case 1:
					switch(SrcPixelSize)
					{
						case 1:
							*pPixel = *pSource;
							break;
						case 3:
						{
							unsigned int Sum = (unsigned int)pSource[0] + (unsigned int)pSource[1] + (unsigned int)pSource[2];
							*pPixel = (unsigned char)Sum/3;
							break;
						}
						default:
							break;
					}
					break;

				case 2:
					if(SrcPixelSize == 2)
						*((short*)pPixel) = *((short*)pSource);
					break;

				case 3:
					switch(SrcPixelSize)
					{
						case 1:
						{
							pPixel[0] = *pSource;
							pPixel[1] = *pSource;
							pPixel[2] = *pSource;
							break;
						}
						case 3:
						{
							*((short*)pPixel) = *((short*)pSource);
							*(pPixel+2) = *(pSource+2);
							break;
						}
						default:
							break;
					}
					break;

				case 4:
					if(SrcPixelSize == 4)
						*((long*)pPixel) = *((long*)pSource);
					break;
			}
			pPixel	+= PixelSize;
			pSource	+= SrcPixelSize;
		}
	}
}

//-------------------------------------------------------------------------------
bool JPEG_IMAGE::CreateImage(unsigned int Width, unsigned int Heigth, unsigned int bpp)
{
	if( _pData )
		free(_pData);
	_pData = malloc( Width * Heigth * bpp );
	
	_BytesPerPixel	= bpp;
	_SizeX			= Width;
	_SizeY			= Heigth;

	return true;
}

//-------------------------------------------------------------------------------
void JPEG_IMAGE::CropTo(unsigned int Width, unsigned int Height, unsigned int nOffsetX, unsigned int nOffsetY, JPEG_IMAGE &imgDest)
{
	unsigned int iDestPixelY;
	unsigned int iDestPixelX;

	imgDest.CreateImage(Width, Height, _BytesPerPixel);

	unsigned char	*pDestBuffer = (unsigned char*)imgDest._pData;

	for( iDestPixelY=nOffsetY; iDestPixelY<Height+nOffsetY; iDestPixelY++ )
	{
		unsigned char	*pLineSrc = ((unsigned char*)_pData) + iDestPixelY*_SizeX*_BytesPerPixel;

		for( iDestPixelX=nOffsetX; iDestPixelX<(Width+nOffsetX); iDestPixelX++ )
		{
			unsigned char		*pSrc = pLineSrc + (iDestPixelX*_BytesPerPixel);

			switch(_BytesPerPixel)
			{
				case 3:
					*pDestBuffer++ = *pSrc++;
				case 2:
					*pDestBuffer++ = *pSrc++;
				case 1:
					*pDestBuffer++ = *pSrc++;
					break;
			}
		}
	}
}

//-------------------------------------------------------------------------------
void JPEG_IMAGE::ClearImage(unsigned int ClearColor)
{
	if(ClearColor)
	{
		if( Bpp() != 1 )
		{
			unsigned int	iPixel;
			unsigned int	nbPixels = _SizeX*_SizeY;
			char			*pScan	= (char*)_pData;

			for(iPixel=0; iPixel<nbPixels; iPixel++)
			{
				memcpy(pScan, &ClearColor, _BytesPerPixel);
				pScan += _BytesPerPixel;
			}
		}
		else
			memset( _pData, ClearColor, Width()*Height() );
	}
	else
		memset( _pData, 0, _SizeX*_SizeY*_BytesPerPixel );
}

//-------------------------------------------------------------------------------
bool JPEG_IMAGE::ReadFromFile(char *pFileName)
{
	const int max_components =3;

	int nSrcWidth	=0;
	int nSrcHeight	=0;
	int nActualComps=0;
	int nReqComps	=max_components; // request RGB image

	_pData =jpgd::decompress_jpeg_image_from_file(pFileName, &_SizeX, &_SizeY, &_BytesPerPixel, nReqComps);

	/*
	jpeg_decompress_struct	cinfo;
	my_error_mgr			jerr;
	FILE					*infile;
	JSAMPARRAY				buffer;
	int						row_stride;

	infile = fopen(pFileName, "rb");
	if( !infile )
		return false;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

	if (setjmp(jerr.setjmp_buffer)) 
	{
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return false;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, infile);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	if( _pData )
		free(_pData);

	_pData = malloc( row_stride * cinfo.output_height );

	unsigned char *pImage = (unsigned char *)_pData;
	while (cinfo.output_scanline < cinfo.output_height)
	{
		unsigned char *pImageFake = pImage;
		jpeg_read_scanlines(&cinfo, (JSAMPARRAY)&pImageFake, 1);
		pImage += row_stride;
	}

	_SizeX			= cinfo.output_width;
	_SizeY			= cinfo.output_height;
	_BytesPerPixel	= cinfo.output_components;

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	fclose(infile);
	*/

	return _pData!=NULL;
}

//-------------------------------------------------------------------------------
bool JPEG_IMAGE::SaveToFile(const char *pFileName, int nCompression)
{
	jpge::params params;
	params.m_quality		= nCompression;
	params.m_subsampling	= jpge::H2V2;
	params.m_two_pass_flag	= true;

	bool bOk =jpge::compress_image_to_jpeg_file(pFileName, _SizeX, _SizeY, _BytesPerPixel, (UInt8*)_pData, params);
	return bOk;

	/*
	jpeg_compress_struct	cinfo;
	my_error_mgr			jerr;
	FILE					*outfile;
	JSAMPROW				row_pointer[1];
	int						row_stride;

	outfile = fopen(pFileName, "wb");
	if( !outfile )
		return false;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

	if (setjmp(jerr.setjmp_buffer)) 
	{
		jpeg_destroy_compress(&cinfo);
		fclose(outfile);
		return false;
	}

	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width		= _SizeX;
	cinfo.image_height		= _SizeY;
	cinfo.input_components	= _BytesPerPixel;
	
	switch(_BytesPerPixel)
	{
		case 1:
			cinfo.in_color_space	= JCS_GRAYSCALE;
			break;
		default:
			cinfo.in_color_space	= JCS_RGB;
			break;
	}

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, 80, TRUE);		// 0 - 100
	jpeg_start_compress(&cinfo, TRUE);
	
	row_stride = _SizeX * _BytesPerPixel;

	while (cinfo.next_scanline < cinfo.image_height) 
	{
		row_pointer[0] = & (((JSAMPROW)_pData)[cinfo.next_scanline * row_stride]);
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	fclose(outfile);
	jpeg_destroy_compress(&cinfo);
	*/

	return true;
}

//-------------------------------------------------------------------------------
void JPEG_IMAGE::InitLanczosTable(void)
{
	if(_pLanczos)
		return;

	_pLanczos = new float[65536];

	float	*pTable = _pLanczos;

	for(int iTable=0; iTable<65536; iTable++)
	{
		float	fValue = (((float)iTable / 65536.0f) * 6.0f) - 3.0f;
		*pTable = Lanczos3(fValue);
		pTable++;
	}
}

//-------------------------------------------------------------------------------
float JPEG_IMAGE::AdjustForCorrectPixelRatio(unsigned int &nWidth, unsigned int &nHeight, bool bExpand)
{
	float	rX = (float)_SizeX / (float)nWidth;		// <1 si on veut du plus grand >1 si on veut du plus petit
	float	rY = (float)_SizeY / (float)nHeight;
	float	rRatio;

	if( bExpand)
	{
		if( rX < rY )
			rRatio = rX;
		else
			rRatio = rY;
	}
	else
	{
		if( rX > rY )
			rRatio = rX;
		else
			rRatio = rY;
	}

	nWidth = (int)((float)_SizeX / rRatio);
	nHeight = (int)((float)_SizeY / rRatio);

	return rRatio;
}

//-------------------------------------------------------------------------------
void JPEG_IMAGE::Resize(JI_RESIZE_METHOD Method, unsigned int Width, unsigned int Height)
{
	switch( Method )
	{
		case RM_POINT_SAMPLE:							// 18ms				x1
			ResizePointSample(Width, Height);
			break;
		case RM_BILINEAR:								// 108ms			x6
			ResizeBilinear(Width, Height);
			break;
		case RM_BOX:									// 457ms			x25
			ResizeBox(Width, Height);
			break;
		case RM_LANCZOS3:								// 2200ms			x122
			ResizeLanczos3(Width, Height);
			break;
	}
}

//-------------------------------------------------------------------------------
void JPEG_IMAGE::ResizeBox(unsigned int Width, unsigned int Height)
{
	unsigned int	iDestPixelX;
	unsigned int	iDestPixelY;

	InitLanczosTable();

	float	fLine		= 0.0f;
	float	fStepX		= (float)_SizeX / (float)Width;
	float	fStepY		= (float)_SizeY / (float)Height;
	float	fHalfSX		= fStepX / 2.0f;
	float	fHalfSY		= fStepX / 2.0f;

	unsigned char	*pNewData	= (unsigned char*)malloc( Width * _SizeY * _BytesPerPixel );
	unsigned char	*pDest		= pNewData;

	// Horizontal Step
	switch( _BytesPerPixel )
	{
		case 1:
		{
			for( iDestPixelY=0; iDestPixelY<_SizeY; iDestPixelY++ )
			{
				unsigned char	*pLineSrc = ((unsigned char*)_pData) + iDestPixelY*_SizeX;

				for( iDestPixelX=0; iDestPixelX<Width; iDestPixelX++ )
				{
					int nPixStart	= (int)((int)(iDestPixelX*fStepX-fHalfSX));
					int nPixEnd		= (int)((int)(iDestPixelX*fStepX+fHalfSX));
					if(nPixStart < 0)		nPixStart = 0;
					if(nPixEnd > (int)_SizeX-1)	
						nPixEnd = _SizeX-1;
	
					float fSum		= 0.0f;
					float fRGB		= 0.0f;
					for(; nPixStart<nPixEnd; nPixStart++)
					{
						float fPixRef = (((float)nPixStart / fStepX) - (float)iDestPixelX);
						fSum += 1.0f;
						unsigned char	*pPixel = pLineSrc + nPixStart;
						fRGB += (float)*pPixel; pPixel++;
					}
					*pDest++ = (unsigned char)((int)fRGB/fSum);
				}
			}
			break;
		}

		case 3:
		{
			for( iDestPixelY=0; iDestPixelY<_SizeY; iDestPixelY++ )
			{
				unsigned char	*pLineSrc = ((unsigned char*)_pData) + iDestPixelY*_SizeX*3;

				for( iDestPixelX=0; iDestPixelX<Width; iDestPixelX++ )
				{
					int nPixStart	= (int)((int)(iDestPixelX*fStepX-fHalfSX));
					int nPixEnd		= (int)((int)(iDestPixelX*fStepX+fHalfSX));
					if(nPixStart < 0)		nPixStart = 0;
					if(nPixEnd > (int)_SizeX-1)	
						nPixEnd = _SizeX-1;
	
					float fSum		= 0.0f;
					float fRed		= 0.0f;
					float fGreen	= 0.0f;
					float fBlue		= 0.0f;
					for(; nPixStart<nPixEnd; nPixStart++)
					{
						float fPixRef = (((float)nPixStart / fStepX) - (float)iDestPixelX);
						fSum += 1.0f;
						unsigned char	*pPixel = pLineSrc + nPixStart*3;

						fRed	+= (float)*pPixel; pPixel++;
						fGreen	+= (float)*pPixel; pPixel++;
						fBlue	+= (float)*pPixel; pPixel++;
					}

					*pDest++ = (unsigned char)((int)fRed/fSum);
					*pDest++ = (unsigned char)((int)fGreen/fSum);
					*pDest++ = (unsigned char)((int)fBlue/fSum);
				}
			}
			break;
		}
	}

	free(_pData);
	_pData	= pNewData;
	_SizeX	= Width;

	pNewData	= (unsigned char*)malloc( Width * Height * _BytesPerPixel );
	pDest		= pNewData;

	// Vertical Step
	switch( _BytesPerPixel )
	{
		case 1:
		{
			for( iDestPixelY=0; iDestPixelY<Height; iDestPixelY++ )
			{
				int nPixStart	= (int)((int)(iDestPixelY*fStepY-fHalfSY));
				int nPixEnd		= (int)((int)(iDestPixelY*fStepY+fHalfSY));
				if(nPixStart < 0)
					nPixStart = 0;
				if(nPixEnd > (int)_SizeY-1)	
					nPixEnd = _SizeY-1;

				for( iDestPixelX=0; iDestPixelX<Width; iDestPixelX++ )
				{
					float fSum		= 0.0f;
					float fRGB		= 0.0f;
					for(int nPix=nPixStart; nPix<nPixEnd; nPix++)
					{
						float fPixRef = (((float)nPix / fStepY) - (float)iDestPixelY);
						fSum += 1.0f;
						unsigned char	*pPixel = ((unsigned char*)_pData) + nPix*_SizeX + iDestPixelX;
						fRGB += (float)*pPixel; pPixel++;
					}

					*pDest++ = (unsigned char)((int)fRGB/fSum);
				}
			}
			break;
		}

		case 3:
		{
			for( iDestPixelY=0; iDestPixelY<Height; iDestPixelY++ )
			{
				int nPixStart	= (int)((int)(iDestPixelY*fStepY-fHalfSY));
				int nPixEnd		= (int)((int)(iDestPixelY*fStepY+fHalfSY));
				if(nPixStart < 0)
					nPixStart = 0;
				if(nPixEnd > (int)_SizeY-1)	
					nPixEnd = _SizeY-1;

				for( iDestPixelX=0; iDestPixelX<Width; iDestPixelX++ )
				{
					float fSum		= 0.0f;
					float fRed		= 0.0f;
					float fGreen	= 0.0f;
					float fBlue		= 0.0f;
					for(int nPix=nPixStart; nPix<nPixEnd; nPix++)
					{
						float fPixRef = (((float)nPix / fStepY) - (float)iDestPixelY);
						fSum += 1.0f;
						unsigned char	*pPixel = ((unsigned char*)_pData) + nPix*_SizeX*3 + iDestPixelX*3;

						fRed	+= (float)*pPixel; pPixel++;
						fGreen	+= (float)*pPixel; pPixel++;
						fBlue	+= (float)*pPixel; pPixel++;
					}

					*pDest++ = (unsigned char)((int)fRed/fSum);
					*pDest++ = (unsigned char)((int)fGreen/fSum);
					*pDest++ = (unsigned char)((int)fBlue/fSum);
				}
			}
			break;
		}
	}

	free(_pData);
	_pData	= pNewData;
	_SizeY	= Height;
}

void JPEG_IMAGE::ResizeMpix(float fDstMegaPixel)
{
	float fSrcMegaPixel =float(_SizeX*_SizeY)/(1000.0f*1000.0f);
	float rRatio =fDstMegaPixel/fSrcMegaPixel;

	float rScale =sqrtf(rRatio);

	int nDstSizeX =int(float(_SizeX)*rScale)&0xFFFFFFF0;		// round to 16
	int nDstSizeY =int(float(_SizeY)*rScale)&0xFFFFFFF0;
	ResizeLanczos3(nDstSizeX, nDstSizeY);
}


//-------------------------------------------------------------------------------
void JPEG_IMAGE::ResizeLanczos3(unsigned int nDstWidth, unsigned int nDstHeight)
{
	int nSrcWidth	=_SizeX;
	int nSrcHeight	=_SizeY;

	const float source_gamma = 1.75f;
	//const char* pFilter = "blackman";//RESAMPLER_DEFAULT_FILTER;
	//const char* pFilter = "lanczos3";
	const char* pFilter = "box";

	const float filter_scale = 1.0f;//.75f;
	int n =3;
	float srgb_to_linear[256];
	for (int i = 0; i < 256; ++i)
	   srgb_to_linear[i] = (float)pow(i * 1.0f/255.0f, source_gamma);

	const int linear_to_srgb_table_size = 4096;
	unsigned char linear_to_srgb[linear_to_srgb_table_size];

	const float inv_linear_to_srgb_table_size = 1.0f / linear_to_srgb_table_size;
	const float inv_source_gamma = 1.0f / source_gamma;

	for (int i = 0; i < linear_to_srgb_table_size; ++i)
	{
	   int k = (int)(255.0f * pow(i * inv_linear_to_srgb_table_size, inv_source_gamma) + .5f);
	   if (k < 0) k = 0; else if (k > 255) k = 255;
	   linear_to_srgb[i] = (unsigned char)k;
	}

	const int max_components =3;
	Resampler* resamplers[max_components];
	std::vector<float> samples[max_components];

	// Now create a Resampler instance for each component to process. The first instance will create new contributor tables, which are shared by the resamplers
	// used for the other components (a memory and slight cache efficiency optimization).
	resamplers[0] = new Resampler(nSrcWidth, nSrcHeight, nDstWidth, nDstHeight, Resampler::BOUNDARY_CLAMP, 0.0f, 1.0f, pFilter, NULL, NULL, filter_scale, filter_scale);
	samples[0].resize(nSrcWidth);
	for (int i = 1; i < n; i++)
	{
	   resamplers[i] = new Resampler(nSrcWidth, nSrcHeight, nDstWidth, nDstHeight, Resampler::BOUNDARY_CLAMP, 0.0f, 1.0f, pFilter, resamplers[0]->get_clist_x(), resamplers[0]->get_clist_y(), filter_scale, filter_scale);
	   samples[i].resize(nSrcWidth);
	}

	//std::vector<unsigned char> dst_image(nDstWidth * n * nDstHeight);
	UInt8* pBitmapIn	=(UInt8*)_pData;
	UInt8* dst_image	=(unsigned char*)malloc( nDstWidth * nDstHeight * _BytesPerPixel );

	const int src_pitch = nSrcWidth * n;
	const int dst_pitch = nDstWidth * n;
	int dst_y = 0;

//	printf("Resampling to %ux%u\n", nDstWidth, nDstHeight);

	for (int src_y = 0; src_y < nSrcHeight; src_y++)
	{
	   const unsigned char* pSrc = &pBitmapIn[src_y * src_pitch];

	   for (int x = 0; x < nSrcWidth; x++)
	   {
		  for (int c = 0; c < n; c++)
		  {
			 if ((c == 3) || ((n == 2) && (c == 1)))
				samples[c][x] = *pSrc++ * (1.0f/255.0f);
			 else
				samples[c][x] = srgb_to_linear[*pSrc++];
		  }
	   }

	   for (int c = 0; c < n; c++)
	   {
		  if (!resamplers[c]->put_line(&samples[c][0]))
		  {
//			 printf("Out of memory!\n");
			 return;// EXIT_FAILURE;
		  }
	   }

	   for ( ; ; )
	   {
		  int comp_index;
		  for (comp_index = 0; comp_index < n; comp_index++)
		  {
			 const float* pOutput_samples = resamplers[comp_index]->get_line();
			 if (!pOutput_samples)
				break;

			 const bool alpha_channel = (comp_index == 3) || ((n == 2) && (comp_index == 1));
//	             assert(dst_y < nDstHeight);
			 unsigned char* pDst = &dst_image[dst_y * dst_pitch + comp_index];

			 for (int x = 0; x < nDstWidth; x++)
			 {
				if (alpha_channel)
				{
				   int c = (int)(255.0f * pOutput_samples[x] + .5f);
				   if (c < 0) c = 0; else if (c > 255) c = 255;
				   *pDst = (unsigned char)c;
				}
				else
				{
				   int j = (int)(linear_to_srgb_table_size * pOutput_samples[x] + .5f);
				   if (j < 0) j = 0; else if (j >= linear_to_srgb_table_size) j = linear_to_srgb_table_size - 1;
				   *pDst = linear_to_srgb[j];
				}

				pDst += n;
			 }
		  }
		  if (comp_index < n)
			 break;

		  dst_y++;
	   }
	}

	// Delete the resamplers.
	for (int i = 0; i < n; i++)
	   delete resamplers[i];

	free(_pData);
	_pData =dst_image;

	_SizeX =nDstWidth;
	_SizeY =nDstHeight;

	/*
	 *
	InitLanczosTable();

	unsigned int	iDestPixelX;
	unsigned int	iDestPixelY;

	float	fLine		= 0.0f;
	float	fStepX		= (float)_SizeX / (float)Width;
	float	fStepY		= (float)_SizeY / (float)Height;

	unsigned char	*pNewData	= (unsigned char*)malloc( Width * _SizeY * _BytesPerPixel );
	unsigned char	*pDest		= pNewData;

	// Horizontal Step
	switch( _BytesPerPixel )
	{
		case 1:
		{
			for( iDestPixelY=0; iDestPixelY<_SizeY; iDestPixelY++ )
			{
				unsigned char	*pLineSrc = ((unsigned char*)_pData) + iDestPixelY*_SizeX;

				for( iDestPixelX=0; iDestPixelX<Width; iDestPixelX++ )
				{
					int nPixStart = (int)(((int)iDestPixelX-LANCZOS_DEPTH)*fStepX);
					int nPixEnd = (int)(((int)iDestPixelX+LANCZOS_DEPTH)*fStepX);
					if(nPixStart < 0)		nPixStart = 0;
					if(nPixEnd > (int)_SizeX-1)	
						nPixEnd = _SizeX-1;
	
					float fSum		= 0.0f;
					float fRGB		= 0.0f;
					for(; nPixStart<nPixEnd; nPixStart++)
					{
						float fPixRef = (((float)nPixStart / fStepX) - (float)iDestPixelX);
						float fLanc = Lanczos3Table( fPixRef );
						if(fLanc <= 0.0f)
							continue;
						fSum += fLanc;
						unsigned char	*pPixel = pLineSrc + nPixStart;
						fRGB += ((float)*pPixel) * fLanc; pPixel++;
					}
					*pDest++ = (unsigned char)((int)fRGB/fSum);
				}
			}
			break;
		}

		case 3:
		{
			for( iDestPixelY=0; iDestPixelY<_SizeY; iDestPixelY++ )
			{
				unsigned char	*pLineSrc = ((unsigned char*)_pData) + iDestPixelY*_SizeX*3;

				for( iDestPixelX=0; iDestPixelX<Width; iDestPixelX++ )
				{
					int nPixStart = (int)(((int)iDestPixelX-LANCZOS_DEPTH)*fStepX);
					int nPixEnd = (int)(((int)iDestPixelX+LANCZOS_DEPTH)*fStepX);
					if(nPixStart < 0)		nPixStart = 0;
					if(nPixEnd > (int)_SizeX-1)	
						nPixEnd = _SizeX-1;
	
					float fSum		= 0.0f;
					float fRed		= 0.0f;
					float fGreen	= 0.0f;
					float fBlue		= 0.0f;

					for(; nPixStart<nPixEnd; nPixStart++)
					{
						float fPixRef = (((float)nPixStart / fStepX) - (float)iDestPixelX);
						float fLanc = Lanczos3Table( fPixRef );
						if(fLanc <= 0.0f)
							continue;
						fSum += fLanc;
						unsigned char	*pPixel = pLineSrc + nPixStart*3;

						fRed	+= ((float)*pPixel) * fLanc; pPixel++;
						fGreen	+= ((float)*pPixel) * fLanc; pPixel++;
						fBlue	+= ((float)*pPixel) * fLanc; pPixel++;
					}

					*pDest++ = (unsigned char)((int)fRed/fSum);
					*pDest++ = (unsigned char)((int)fGreen/fSum);
					*pDest++ = (unsigned char)((int)fBlue/fSum);
				}
			}
			break;
		}
	}

	free(_pData);
	_pData	= pNewData;
	_SizeX	= Width;

	pNewData	= (unsigned char*)malloc( Width * Height * _BytesPerPixel );
	pDest		= pNewData;

	// Vertical Step
	switch( _BytesPerPixel )
	{
		case 1:
		{
			for( iDestPixelY=0; iDestPixelY<Height; iDestPixelY++ )
			{
				int nPixStart	= (int)(((int)iDestPixelY-LANCZOS_DEPTH)*fStepY);
				int nPixEnd		= (int)(((int)iDestPixelY+LANCZOS_DEPTH)*fStepY);
				if(nPixStart < 0)
					nPixStart = 0;
				if(nPixEnd > (int)_SizeY-1)	
					nPixEnd = _SizeY-1;

				for( iDestPixelX=0; iDestPixelX<Width; iDestPixelX++ )
				{
					float fSum		= 0.0f;
					float fRGB		= 0.0f;
					for(int nPix=nPixStart; nPix<nPixEnd; nPix++)
					{
						float fPixRef = (((float)nPix / fStepY) - (float)iDestPixelY);
						float fLanc = Lanczos3Table( fPixRef );
						if(fLanc <= 0.0f)
							continue;
						fSum += fLanc;
						unsigned char	*pPixel = ((unsigned char*)_pData) + nPix*_SizeX + iDestPixelX;
						fRGB += ((float)*pPixel) * fLanc; pPixel++;
					}

					*pDest++ = (unsigned char)((int)fRGB/fSum);
				}
			}
			break;
		}

		case 3:
		{
			for( iDestPixelY=0; iDestPixelY<Height; iDestPixelY++ )
			{
				int nPixStart	= (int)(((int)iDestPixelY-LANCZOS_DEPTH)*fStepY);
				int nPixEnd		= (int)(((int)iDestPixelY+LANCZOS_DEPTH)*fStepY);
				if(nPixStart < 0)
					nPixStart = 0;
				if(nPixEnd > (int)_SizeY-1)	
					nPixEnd = _SizeY-1;

				for( iDestPixelX=0; iDestPixelX<Width; iDestPixelX++ )
				{
					float fSum		= 0.0f;
					float fRed		= 0.0f;
					float fGreen	= 0.0f;
					float fBlue		= 0.0f;
					for(int nPix=nPixStart; nPix<nPixEnd; nPix++)
					{
						float fPixRef = (((float)nPix / fStepY) - (float)iDestPixelY);
						float fLanc = Lanczos3Table( fPixRef );
						if(fLanc <= 0.0f)
							continue;
						fSum += fLanc;
						unsigned char	*pPixel = ((unsigned char*)_pData) + nPix*_SizeX*3 + iDestPixelX*3;

						fRed	+= ((float)*pPixel) * fLanc; pPixel++;
						fGreen	+= ((float)*pPixel) * fLanc; pPixel++;
						fBlue	+= ((float)*pPixel) * fLanc; pPixel++;
					}

					*pDest++ = (unsigned char)((int)fRed/fSum);
					*pDest++ = (unsigned char)((int)fGreen/fSum);
					*pDest++ = (unsigned char)((int)fBlue/fSum);
				}
			}
			break;
		}
	}

	free(_pData);
	_pData	= pNewData;
	_SizeY	= Height;
	*/

}

//-------------------------------------------------------------------------------
void JPEG_IMAGE::ResizeBilinear(unsigned int Width, unsigned int Height)
{
	unsigned int	iDestPixelX;
	unsigned int	iDestPixelY;

	float	fLine		= 0.0f;
	float	fStepX		= (float)_SizeX / (float)Width;
	float	fStepY		= (float)_SizeY / (float)Height;
	char	*pNewData	= (char*)malloc( Width * Height * _BytesPerPixel );
	char	*pDest		= pNewData;

	switch( _BytesPerPixel )
	{
		case 1:
			for( iDestPixelY=0; iDestPixelY<Height; iDestPixelY++)
			{
				float	fLineFac	= fLine - floorf(fLine);
				float	fInvLineFac	= 1.0f - fLineFac;

				unsigned char	*pLineSrc1	= ((unsigned char*)_pData) + ((unsigned int)floorf(fLine))*_SizeX;
				unsigned char	*pLineSrc2	= pLineSrc1 + _SizeX;
				unsigned char	*pPixSource1;
				unsigned char	*pPixSource2;
				unsigned char	*pPixSource3;
				unsigned char	*pPixSource4;
				float			fCurX		= 0.0f;

				if(floorf(fLine) >= (_SizeY-1))
				{
					pLineSrc2	= pLineSrc1;
					fLineFac	= 0.0f;
					fInvLineFac	= 1.0f;
				}

				for(iDestPixelX=0; iDestPixelX<Width; iDestPixelX++)
				{
					if(floorf(fCurX) >= (_SizeX-1))
						break;

					float	fPixel		= fCurX - floorf(fCurX);
					float	fInvPixel	= 1.0f - fPixel;
					
					pPixSource1	= pLineSrc1 + (unsigned int)floorf(fCurX);
					pPixSource2	= pPixSource1 + 1;
					pPixSource3	= pLineSrc2 + (unsigned int)floorf(fCurX);
					pPixSource4	= pPixSource3 + 1;

					*pDest = (unsigned char)
								(
									(*pPixSource1*fInvLineFac*fInvPixel) + 
									(*pPixSource2*fInvLineFac*fPixel) + 
									(*pPixSource3*fLineFac*fInvPixel) + 
									(*pPixSource4*fLineFac*fPixel)
								); 
					pDest++;
					fCurX += fStepX;
				}
				for(; iDestPixelX<Width; iDestPixelX++)
				{
					*pDest++ = (unsigned char)((*pPixSource2*fInvLineFac) + (*pPixSource4*fLineFac)); 
				}
				
				fLine += fStepY;
			}
			break;

		case 3:
			for( iDestPixelY=0; iDestPixelY<Height; iDestPixelY++)
			{
				float	fLineFac	= fLine - floorf(fLine);
				float	fInvLineFac	= 1.0f - fLineFac;

				unsigned char	*pLineSrc1	= ((unsigned char*)_pData) + ((unsigned int)floorf(fLine))*_SizeX*3;
				unsigned char	*pLineSrc2	= pLineSrc1 + (_SizeX*3);
				unsigned char	*pPixSource1;
				unsigned char	*pPixSource2;
				unsigned char	*pPixSource3;
				unsigned char	*pPixSource4;
				float			fCurX		= 0.0f;

				if(floorf(fLine) >= (_SizeY-1))
				{
					pLineSrc2	= pLineSrc1;
					fLineFac	= 0.0f;
					fInvLineFac	= 1.0f;
				}

				for(iDestPixelX=0; iDestPixelX<Width; iDestPixelX++)
				{
					if(floorf(fCurX) >= (_SizeX-1))
						break;
					float			fPixel			= fCurX - floorf(fCurX);
					float			fInvPixel		= 1.0f - fPixel;
					float			ff1				= fInvLineFac*fInvPixel;
					float			ff2				= fInvLineFac*fPixel;
					float			ff3				= fLineFac*fInvPixel;
					float			ff4				= 1.0f-ff1-ff2-ff3;

					pPixSource1	= pLineSrc1 + ((unsigned int)floorf(fCurX))*3;
					pPixSource2	= pPixSource1 + 3;
					pPixSource3	= pLineSrc2 + ((unsigned int)floorf(fCurX))*3;
					pPixSource4	= pPixSource3 + 3;

					*pDest++ = (unsigned char)((pPixSource1[0]*ff1) + (pPixSource2[0]*ff2) + (pPixSource3[0]*ff3) + (pPixSource4[0]*ff4));
					*pDest++ = (unsigned char)((pPixSource1[1]*ff1) + (pPixSource2[1]*ff2) + (pPixSource3[1]*ff3) + (pPixSource4[1]*ff4));
					*pDest++ = (unsigned char)((pPixSource1[2]*ff1) + (pPixSource2[2]*ff2) + (pPixSource3[2]*ff3) + (pPixSource4[2]*ff4));
					fCurX += fStepX;
				}
				for(; iDestPixelX<Width; iDestPixelX++)
				{
					*pDest++ = (unsigned char)((pPixSource2[0]*fInvLineFac) + (pPixSource4[0]*fLineFac));
					*pDest++ = (unsigned char)((pPixSource2[1]*fInvLineFac) + (pPixSource4[1]*fLineFac));
					*pDest++ = (unsigned char)((pPixSource2[2]*fInvLineFac) + (pPixSource4[2]*fLineFac));
				}
				fLine += fStepY;
			}
			break;
	}

	free(_pData);
	_pData	= pNewData;
	_SizeX	= Width;
	_SizeY	= Height;
}

//-------------------------------------------------------------------------------
void JPEG_IMAGE::ResizePointSample(unsigned int Width, unsigned int Height)
{
	char	*pNewData	= (char*)malloc( Width * Height * _BytesPerPixel );
	char	*pDest		= pNewData;
	char	*pSource	= (char*)_pData;

	unsigned int iDestPixelX;
	unsigned int iDestPixelY;

	float	fStepX	= (float)_SizeX / (float)Width;
	float	fStepY	= (float)_SizeY / (float)Height;
	float	fCurX	= 0.0f;
	float	fCurY	= 0.0f;

	switch( _BytesPerPixel )
	{
		case 1:
			for( iDestPixelY=0; iDestPixelY<Height; iDestPixelY++)
			{
				fCurX = 0.0f;
				unsigned int nLine = (((unsigned int)fCurY)*_SizeX);
				for(iDestPixelX=0; iDestPixelX<Width; iDestPixelX++)
				{
					pSource = ((char*)_pData) + (nLine + (unsigned int)fCurX);
					*pDest = pSource[0]; pDest++;
					fCurX += fStepX;
				}
				fCurY += fStepY;
			}
			break;

		case 3:
			for( iDestPixelY=0; iDestPixelY<Height; iDestPixelY++)
			{
				fCurX = 0.0f;
				unsigned int nLine = (((unsigned int)fCurY)*_SizeX);
				for(iDestPixelX=0; iDestPixelX<Width; iDestPixelX++)
				{
					pSource = ((char*)_pData) + (nLine + (unsigned int)fCurX) * 3;
					*pDest = pSource[0]; pDest++;
					*pDest = pSource[1]; pDest++;
					*pDest = pSource[2]; pDest++;

					fCurX += fStepX;
				}
				fCurY += fStepY;
			}
			break;

	}

	free(_pData);
	_pData	= pNewData;
	_SizeX	= Width;
	_SizeY	= Height;
}






