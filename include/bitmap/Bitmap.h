#pragma once

#include "Types.h"
#include <functional>

namespace Framework
{
	class CColor
	{
	public:
		CColor()
			: r(0), g(0), b(0), a(0)
		{
		
		}

		CColor(uint8 r, uint8 g, uint8 b, uint8 a)
			: r(r), g(g), b(b), a(a)
		{

		}

		uint8	r;
		uint8	g;
		uint8	b;
		uint8	a;
	};

	class CBitmap
	{
	public:
						CBitmap();
						CBitmap(const CBitmap&);
						CBitmap(CBitmap&&);
						CBitmap(unsigned int, unsigned int, unsigned int);
						~CBitmap();

		CBitmap&		operator =(const CBitmap&);
		CBitmap&		operator =(CBitmap&&);

		void			Allocate(unsigned int, unsigned int, unsigned int);
		void			Reset();

		bool			IsEmpty() const;

		unsigned int	GetPixelSize() const;
		unsigned int	GetPixelsSize() const;
		unsigned int	GetPitch() const;
		unsigned int	GetWidth() const;
		unsigned int	GetHeight() const;
		unsigned int	GetBitsPerPixel() const;
		CColor			GetPixel(unsigned int, unsigned int) const;
		uint8*			GetPixels() const;
		void			Blit(const CBitmap&, unsigned int, unsigned int);

		CBitmap			AddAlphaChannel(uint8 alphaValue) const;
		CBitmap			Resize(unsigned int, unsigned int) const;
		CBitmap			ResizeCanvas(unsigned int, unsigned int) const;

	private:
		void			CopyFrom(const CBitmap&);
		void			MoveFrom(CBitmap&&);

		unsigned int	m_width;
		unsigned int	m_height;
		unsigned int	m_bpp;
		uint8*			m_pixels;
	};

}