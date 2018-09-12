#pragma once

#include <afxwin.h>

#include <vector>

#include <util/common/math/complex.h>
#include <util/common/plot/plot.h>
#include <util/common/math/fft.h>

namespace model
{

    /*****************************************************/
    /*                     params                        */
    /*****************************************************/

    struct parameters
    {
        // system params
        double snr;
        double smin, smax;
    };

    inline parameters make_default_parameters()
    {
        parameters p =
        {
            // system params
            1.0,
            0.01,
            0.3
        };
        return p;
    }

    inline plot::drawable::ptr_t make_root_drawable
    (
        std::vector < plot::drawable::ptr_t > layers
    )
    {
        using namespace plot;

        return viewporter::create(
            layer_drawable::create(layers),
            make_viewport_mapper(world_t{-1, 1, -1, 1})
        );
    }

    /*****************************************************/
    /*                     data                          */
    /*****************************************************/

    struct bitmap
    {
        size_t h, w;
        std::vector < std::vector < math::complex <> > > data;
        bitmap & reshape(size_t n, size_t m)
        {
            h = n; w = m;
            data.resize(n);
            for (size_t i = 0; i < n; ++i)
                data[i].resize(m, 0);
            return *this;
        }
        bitmap & resize(size_t n, size_t m)
        {
            bitmap b; b.reshape(std::fmax(n, h), m);
            auto & buf = b.data;
            double sm = (double) w / m;
            double sn = (double) h / n;

            for (size_t r = 0; r < h; ++r)
            for (size_t i = 0; i < m; ++i)
            {
                double x = sm * i;
                size_t i0 = (size_t) std::floor(x);
                if (i0 + 1 >= w)
                {
                    buf[r][i] = data[r][w - 1];
                }
                else
                {
                    auto k = data[r][i0 + 1] - data[r][i0];
                    auto b = data[r][i0] - k * i0;
                    buf[r][i] = k * x + b;
                }
            }

            data = std::move(buf);

            std::vector < math::complex <> > buf0(std::fmax(h, n));

            for (size_t c = 0; c < m; ++c)
            {
                for (size_t i = 0; i < std::fmax(h, n); ++i)
                    buf0[i] = data[i][c];
                for (size_t i = 0; i < n; ++i)
                {
                    double x = sn * i;
                    size_t i0 = (size_t) std::floor(x);
                    if (i0 + 1 >= h)
                    {
                        data[i][c] = buf0[h - 1];
                    }
                    else
                    {
                        auto k = buf0[i0 + 1] - buf0[i0];
                        auto b = buf0[i0] - k * i0;
                        data[i][c] = k * x + b;
                    }
                }
            }

            data.resize(n);
            h = n;
            w = m;

            return *this;
        }
        bitmap & transp()
        {
            for (size_t i = 0; i < h; ++i)
            for (size_t j = 0; j < i; ++j)
            {
                std::swap(data[i][j], data[j][i]);
            }
            return *this;
        }
        bitmap & rearrange()
        {
            for (size_t i = 0; i < h / 2; ++i)
            for (size_t j = 0; j < h / 2; ++j)
            {
                std::swap(data[i][j], data[i + h / 2][j + h / 2]);
                std::swap(data[i + h / 2][j], data[i][j + h / 2]);
            }
            return *this;
        }
        bitmap & fourier_of(bitmap & o, bool inv = false)
        {
            ASSERT(o.w == o.h);

            *this = o;

            for (size_t i = 0; i < h; ++i)
                fourier(data[i].data(), w, inv ? 1 : -1);
            transp();
            for (size_t i = 0; i < h; ++i)
                fourier(data[i].data(), w, inv ? 1 : -1);
            transp();

            return *this;
        }
        bitmap & to_cbitmap(CBitmap & bmp, double d, bool abs)
        {
            std::vector < COLORREF > buf(h * w);
            for (size_t i = 0; i < h; ++i)
            for (size_t j = 0; j < w; ++j)
            {
                double v = abs ? math::norm(data[i][j]) : std::fmax(0, data[i][j].re);
                BYTE c = (BYTE) std::fmin(v / d * 255, 255);
                buf[w * i + j] = RGB(c, c, c);
            }
            bmp.DeleteObject();
            bmp.CreateBitmap(w, h, 1, sizeof(COLORREF) * 8, (LPCVOID) buf.data());
            bmp.SetBitmapDimension(w, h);
            return *this;
        }
        bitmap & log(double n)
        {
            for (size_t i = 0; i < h; ++i)
            for (size_t j = 0; j < w; ++j)
                data[i][j] = std::log10(math::sqnorm(data[i][j]) / n / n);
            return *this;
        }
        bitmap & from_cbitmap(CBitmap & bmp)
        {
            BITMAP bmpd;
            bmp.GetBitmap(&bmpd);
            w = bmpd.bmWidth; h = bmpd.bmHeight;
            reshape(h, w);
            CDC dc; dc.CreateCompatibleDC(nullptr);
            dc.SelectObject(&bmp);
            for (size_t i = 0; i < h; ++i)
            for (size_t j = 0; j < w; ++j)
            {
                auto p = dc.GetPixel(j, i);
                data[i][j] = (0.299 * (p & 0xff) +
                              0.587 * ((p >> 8) & 0xff) +
                              0.114 * ((p >> 16) & 0xff)) / 255.;
            }
            return *this;
        }
    };

    /*****************************************************/
    /*                     model                         */
    /*****************************************************/

    enum stage { stage_no, stage_source, stage_fourier, stage_final };

    struct model_data
    {
        parameters params;
        model::stage stage;
        CBitmap csource;
        bitmap source;
        CBitmap cnoised;
        bitmap noised;
        CBitmap cfourier;
        bitmap fourier;
        CBitmap cfiltered;
        bitmap filtered;
    };
}