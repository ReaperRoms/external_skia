/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "gm.h"
#include "SkCanvas.h"
#include "SkGradientShader.h"
#include "SkPath.h"
#include "SkRandom.h"

int make_bm(SkBitmap* bm, int height) {
    static const int kRadius = 22;
    static const int kMargin = 8;
    static const SkScalar kStartAngle = 0;
    static const SkScalar kDAngle = 25;
    static const SkScalar kSweep = 320;
    static const SkScalar kThickness = 8;

    int count = (height / (2 * kRadius + kMargin));
    height = count * (2 * kRadius + kMargin);

    bm->allocN32Pixels(2 * (kRadius + kMargin), height);
    SkRandom random;

    SkCanvas wholeCanvas(*bm);
    wholeCanvas.clear(0x00000000);

    SkScalar angle = kStartAngle;
    for (int i = 0; i < count; ++i) {
        SkPaint paint;
        // The sw rasterizer disables AA for large canvii. So we make a small canvas for each draw.
        SkBitmap smallBM;
        SkIRect subRect = SkIRect::MakeXYWH(0, i * (kMargin + 2 * kRadius),
                                            2 * kRadius + kMargin, 2 * kRadius + kMargin);
        bm->extractSubset(&smallBM, subRect);
        SkCanvas canvas(smallBM);
        canvas.translate(kMargin + kRadius, kMargin + kRadius);

        paint.setAntiAlias(true);
        paint.setColor(random.nextU() | 0xFF000000);
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setStrokeWidth(kThickness);
        paint.setStrokeCap(SkPaint::kRound_Cap);
        SkScalar radius = kRadius - kThickness / 2;
        SkRect bounds = SkRect::MakeLTRB(-radius, -radius, radius, radius);

        canvas.drawArc(bounds, angle, kSweep, false, paint);
        angle += kDAngle;
    }
    bm->setImmutable();
    return count;
}

class TallStretchedBitmapsGM : public skiagm::GM {
public:
    TallStretchedBitmapsGM() {}

protected:
    SkString onShortName() SK_OVERRIDE {
        return SkString("tall_stretched_bitmaps");
    }

    SkISize onISize() SK_OVERRIDE {
        return SkISize::Make(750, 750);
    }

    void onOnceBeforeDraw() SK_OVERRIDE {
        for (size_t i = 0; i < SK_ARRAY_COUNT(fTallBmps); ++i) {
            int h = SkToInt((4 + i) * 1024);

            fTallBmps[i].fItemCnt = make_bm(&fTallBmps[i].fBmp, h);
        }
    }

    void onDraw(SkCanvas* canvas) SK_OVERRIDE {
        canvas->scale(1.3f, 1.3f);
        for (size_t i = 0; i < SK_ARRAY_COUNT(fTallBmps); ++i) {
            SkASSERT(fTallBmps[i].fItemCnt > 10);
            SkBitmap bmp = fTallBmps[i].fBmp;
            // Draw the last 10 elements of the bitmap.
            int startItem = fTallBmps[i].fItemCnt - 10;
            int itemHeight = bmp.height() / fTallBmps[i].fItemCnt;
            SkIRect subRect = SkIRect::MakeLTRB(0, startItem * itemHeight,
                                               bmp.width(), bmp.height());
            SkRect dstRect = SkRect::MakeWH(SkIntToScalar(bmp.width()), 10.f * itemHeight);
            SkPaint paint;
            paint.setFilterLevel(SkPaint::kLow_FilterLevel);
            canvas->drawBitmapRect(bmp, &subRect, dstRect, &paint);
            canvas->translate(SkIntToScalar(bmp.width() + 10), 0);
        }
    }    

    uint32_t onGetFlags() const SK_OVERRIDE {
        // This GM causes issues in replay modes.
        return kSkipTiled_Flag | kNoBBH_Flag | kSkipPicture_Flag;
    }

private:
    struct {
        SkBitmap fBmp;
        int      fItemCnt;
    } fTallBmps[8];
    typedef skiagm::GM INHERITED;
};

//////////////////////////////////////////////////////////////////////////////

DEF_GM(return SkNEW(TallStretchedBitmapsGM);)

