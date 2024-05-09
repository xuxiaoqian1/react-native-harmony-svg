// from ArkUI "frameworks/core/components_ng/svg/parse/svg_graphic.h"

#pragma once
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include "SvgNode.h"
#include "RNOH/CppComponentInstance.h"
#include "utils/StringUtils.h"
#include "utils/SvgAttributesParser.h"
#include "utils/StringUtils.h"
#include "utils/Utils.h"

namespace rnoh {

class SvgGraphic : public SvgNode {
public:
    SvgGraphic() : SvgNode() {
        fillBrush_ = OH_Drawing_BrushCreate();
        strokePen_ = OH_Drawing_PenCreate();
        path_ = OH_Drawing_PathCreate();
    }
    //     virtual ~SvgGraphic() override = default;
    ~SvgGraphic() override {
        OH_Drawing_BrushDestroy(fillBrush_);
        OH_Drawing_PenDestroy(strokePen_);
        OH_Drawing_PathDestroy(path_);
    }


    void OnDraw(OH_Drawing_Canvas *canvas) override;

    // temporary
    void setBrushColor(const uint32_t fill,double fillOpacity) { 
//         OH_Drawing_BrushSetColor(fillBrush_, fill);
        attributes_.fillState.SetColor(Color(fill));
        attributes_.fillState.SetOpacity(fillOpacity);
    }
    void setStrokColor(const uint32_t fill) {
        attributes_.strokeState.SetColor(Color(fill));
    }
    void setStrokeOpacity(const double strokeOpacity) {
        attributes_.strokeState.SetOpacity(std::clamp(strokeOpacity, 0.0, 1.0));
    }
    void setStrokeLineWith(const std::string strokeWidth) {
        attributes_.strokeState.SetLineWidth(Dimension(vpToPx(StringUtils::StringToDouble(strokeWidth))));
    }
    void setStrokeLineCap(const int strokeLinecap) {
        attributes_.strokeState.SetLineCap(SvgAttributesParser::GetLineCapStyle(std::to_string(strokeLinecap)));
    }
    void setStrokeLineJoin(const int strokeLinejoin) {
        attributes_.strokeState.SetLineJoin(SvgAttributesParser::GetLineJoinStyle(std::to_string(strokeLinejoin)));
    }
    void setStrokeDasharray(const std::vector<std::string> strokeDasharray) {
        std::vector<double> lineDashVector;
        lineDashVector = StringUtils::stringVectorToDoubleVector(strokeDasharray);
        attributes_.strokeState.SetLineDash(lineDashVector);
    }
    void setStrokeDashoffset(const double strokeDashoffset) {
        attributes_.strokeState.SetLineDashOffset(vpToPx(strokeDashoffset));
    }
    void setStrokeMiterlimit(const double strokeMiterlimit) {
        if (GreatOrEqual(vpToPx(strokeMiterlimit), 1.0)) {
            attributes_.strokeState.SetMiterLimit(vpToPx(strokeMiterlimit));
        }
    }
    
protected:
    OH_Drawing_Path *path_;
    OH_Drawing_Brush *fillBrush_;
    OH_Drawing_Pen *strokePen_;

    // Use Brush to draw fill
    void OnGraphicFill(OH_Drawing_Canvas *canvas) {
        auto smoothEdge = GetSmoothEdge();
        if (GreatNotEqual(smoothEdge, 0.0f)) {
            LOG(INFO) << "[svg] OnGraphicFill1";
            auto *filter = OH_Drawing_FilterCreate();
            auto *maskFilter =
                OH_Drawing_MaskFilterCreateBlur(OH_Drawing_BlurType::NORMAL, static_cast<double>(smoothEdge), false);
            OH_Drawing_FilterSetMaskFilter(filter, maskFilter);

            /* copy constructor missing */
            auto tmpFillBrush = fillBrush_;
            OH_Drawing_BrushSetFilter(tmpFillBrush, filter);
            OH_Drawing_CanvasAttachBrush(canvas, tmpFillBrush);
            OH_Drawing_CanvasDrawPath(canvas, path_);
            OH_Drawing_CanvasDetachBrush(canvas);

            OH_Drawing_FilterDestroy(filter);
            OH_Drawing_MaskFilterDestroy(maskFilter);
            OH_Drawing_BrushDestroy(tmpFillBrush);
        } else {
            LOG(INFO) << "[svg] OnGraphicFill2";
            OH_Drawing_CanvasAttachBrush(canvas, fillBrush_);
            OH_Drawing_CanvasDrawPath(canvas, path_);
            OH_Drawing_CanvasDetachBrush(canvas);
        }
    }

    // Use Pen to draw stroke
    void OnGraphicStroke(OH_Drawing_Canvas *canvas) {
        auto smoothEdge = GetSmoothEdge();
        if (GreatNotEqual(smoothEdge, 0.0f)) {
            LOG(INFO) << "[svg] OnGraphicStroke1";
            auto *filter = OH_Drawing_FilterCreate();
            auto *maskFilter =
                OH_Drawing_MaskFilterCreateBlur(OH_Drawing_BlurType::NORMAL, static_cast<double>(smoothEdge), false);
            OH_Drawing_FilterSetMaskFilter(filter, maskFilter);

            /* copy constructor missing */
            auto tmpStrokePen = strokePen_;
            OH_Drawing_PenSetFilter(tmpStrokePen, filter);
            OH_Drawing_CanvasAttachPen(canvas, tmpStrokePen);
            OH_Drawing_CanvasDrawPath(canvas, path_);
            OH_Drawing_CanvasDetachPen(canvas);

            OH_Drawing_FilterDestroy(filter);
            OH_Drawing_MaskFilterDestroy(maskFilter);
            OH_Drawing_PenDestroy(tmpStrokePen);
        } else {
            LOG(INFO) << "[svg] OnGraphicStroke2";
            OH_Drawing_CanvasAttachPen(canvas, strokePen_);
            OH_Drawing_CanvasDrawPath(canvas, path_);
            OH_Drawing_CanvasDetachPen(canvas);
        }
    }

    bool UpdateFillStyle(bool antiAlias = true);
    bool UpdateStrokeStyle(bool antiAlias = true);
    void UpdateGradient();
    void SetGradientStyle(double opacity);
    void UpdateLineDash();

private:
    // TODO void UpdateColorFilter(OH_Drawing_Filter *filter);
};

} // namespace rnoh