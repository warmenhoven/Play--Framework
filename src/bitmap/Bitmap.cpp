#include <stdlib.h>
#include <assert.h>
#include <stdexcept>
#include <string.h>
#include "bitmap/Bitmap.h"

using namespace Framework;

CBitmap::CBitmap()
: m_pixels(nullptr)
, m_width(0)
, m_height(0)
, m_bpp(0)
{

}

CBitmap::CBitmap(const CBitmap& src)
: m_pixels(nullptr)
, m_width(0)
, m_height(0)
, m_bpp(0)
{
	CopyFrom(src);
}

CBitmap::CBitmap(CBitmap&& src)
: m_pixels(nullptr)
, m_width(0)
, m_height(0)
, m_bpp(0)
{
	MoveFrom(std::move(src));
}

CBitmap::CBitmap(unsigned int nWidth, unsigned int nHeight, unsigned int nBPP)
: m_pixels(nullptr)
, m_width(0)
, m_height(0)
, m_bpp(0)
{
	Allocate(nWidth, nHeight, nBPP);
}

CBitmap::~CBitmap()
{
	Reset();
}

CBitmap& CBitmap::operator =(const CBitmap& src)
{
	if(&src == this)
	{
		return (*this);
	}

	CopyFrom(src);

	return (*this);
}

CBitmap& CBitmap::operator =(CBitmap&& src)
{
	Reset();
	MoveFrom(std::move(src));
	return (*this);
}

void CBitmap::Allocate(unsigned int width, unsigned int height, unsigned int bpp)
{
	assert(m_pixels == nullptr);

	m_width		= width;
	m_height	= height;
	m_bpp		= bpp;
	m_pixels	= new uint8[GetPixelsSize()];
}

void CBitmap::Reset()
{
	delete [] m_pixels;
	m_pixels = nullptr;

	m_width = 0;
	m_height = 0;
	m_bpp = 0;
}

bool CBitmap::IsEmpty() const
{
	return m_pixels == nullptr;
}

uint8* CBitmap::GetPixels() const
{
	assert(m_pixels != nullptr);
	return m_pixels;
}

CColor CBitmap::GetPixel(unsigned int x, unsigned int y) const
{
	if(m_pixels == nullptr)
	{
		return CColor(0, 0, 0, 0);
	}

	if(x > m_width) return CColor(0, 0, 0, 0);
	if(y > m_height) return CColor(0, 0, 0, 0);

	uint8* pixelPtr = (m_pixels + (GetPitch() * y) + (GetPixelSize() * x));
	switch(m_bpp)
	{
	case 32:
		return CColor(pixelPtr[0], pixelPtr[1], pixelPtr[2], pixelPtr[3]);
		break;
	case 24:
		return CColor(pixelPtr[0], pixelPtr[1], pixelPtr[2], 0);
		break;
	default:
		throw std::runtime_error("Unknown bit depth.");
		break;
	}
}

unsigned int CBitmap::GetWidth() const
{
	return m_width;
}

unsigned int CBitmap::GetHeight() const
{
	return m_height;
}

unsigned int CBitmap::GetBitsPerPixel() const
{
	return m_bpp;
}

unsigned int CBitmap::GetPixelSize() const
{
	return (m_bpp + 7) / 8;
}

unsigned int CBitmap::GetPixelsSize() const
{
	unsigned int nSize = m_width * m_height * m_bpp;
	return ((nSize + 7) / 8);
}

unsigned int CBitmap::GetPitch() const
{
	return (((m_width * m_bpp) + 7) / 8);
}

void CBitmap::Blit(const CBitmap& Src, unsigned int nLeft, unsigned int nTop)
{
	CBitmap& Dst = (*this);

	unsigned int nWidth		= Src.GetWidth();
	unsigned int nHeight	= Src.GetHeight();

	if(&Dst == &Src)
	{
		throw std::exception();
	}

	if((nWidth + nLeft) > Dst.GetWidth())
	{
		throw std::exception();
	}

	if((nHeight + nTop) > Dst.GetHeight())
	{
		throw std::exception();
	}

	if(Dst.GetBitsPerPixel() != Src.GetBitsPerPixel())
	{
		throw std::exception();
	}

	size_t nSrcPitch = Src.GetPitch();
	size_t nDstPitch = Dst.GetPitch();
	uint8* pSrcPtr = Src.GetPixels();
	uint8* pDstPtr = Dst.GetPixels() + (nDstPitch * nTop) + (nLeft * Dst.GetBitsPerPixel() / 8);

	for(unsigned int j = 0; j < nHeight; j++)
	{
		memcpy(pDstPtr, pSrcPtr, nSrcPitch);
		pDstPtr += nDstPitch;
		pSrcPtr += nSrcPitch;
	}
}

CBitmap CBitmap::AddAlphaChannel(uint8 alphaValue) const
{
	assert(m_bpp == 24);
	CBitmap result(m_width, m_height, 32);

	uint32 pixelCount = m_width * m_height;
	for(uint32 i = 0; i < pixelCount; i++)
	{
		result.m_pixels[(i * 4) + 0] = m_pixels[(i * 3) + 0];
		result.m_pixels[(i * 4) + 1] = m_pixels[(i * 3) + 1];
		result.m_pixels[(i * 4) + 2] = m_pixels[(i * 3) + 2];
		result.m_pixels[(i * 4) + 3] = alphaValue;
	}

	return result;
}

CBitmap CBitmap::Resize(unsigned int newWidth, unsigned int newHeight) const
{
	CBitmap result(newWidth, newHeight, m_bpp);

	unsigned int srcPitch = GetPitch();
	unsigned int dstPitch = result.GetPitch();
	unsigned int pixelSize = GetPixelSize();

	auto dstPtr = result.m_pixels;
	for(unsigned int y = 0; y < newHeight; y++)
	{
		unsigned int sampleY = (y * m_height) / newHeight;
		for(unsigned int x = 0; x < newWidth; x++)
		{
			unsigned int sampleX = (x * m_width) / newWidth;

			for(unsigned int i = 0; i < pixelSize; i++)
			{
				uint8 pixelSrc = m_pixels[(sampleX * pixelSize) + (sampleY * srcPitch) + i];
				dstPtr[(x * pixelSize) + i] = pixelSrc;
			}
		}
		dstPtr += dstPitch;
	}

	return result;
}

CBitmap CBitmap::ResizeCanvas(unsigned int newWidth, unsigned int newHeight) const
{
	CBitmap result(newWidth, newHeight, m_bpp);

	unsigned int srcPitch = GetPitch();
	unsigned int dstPitch = result.GetPitch();
	unsigned int copyPitch = std::min<unsigned int>(srcPitch, dstPitch);
	unsigned int copyHeight = std::min<unsigned int>(newHeight, m_height);

	auto srcPtr = m_pixels;
	auto dstPtr = result.m_pixels;
	for(unsigned int y = 0; y < copyHeight; y++)
	{
		memcpy(dstPtr, srcPtr, copyPitch);
		srcPtr += srcPitch;
		dstPtr += dstPitch;
	}

	return result;
}

void CBitmap::CopyFrom(const CBitmap& src)
{
	if(src.GetPixelsSize() != GetPixelsSize())
	{
		m_pixels = reinterpret_cast<uint8*>(realloc(m_pixels, src.GetPixelsSize()));
	}

	m_width		= src.GetWidth();
	m_height	= src.GetHeight();
	m_bpp		= src.GetBitsPerPixel();

	if(GetPixelsSize() != 0)
	{
		memcpy(m_pixels, src.GetPixels(), GetPixelsSize());
	}
}

void CBitmap::MoveFrom(CBitmap&& src)
{
	std::swap(src.m_pixels,	m_pixels);
	std::swap(src.m_width,	m_width);
	std::swap(src.m_height,	m_height);
	std::swap(src.m_bpp,	m_bpp);
}