#include "shell/browser/ui/view_utils.h"

#include <string>

#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "shell/common/gin_converters/gfx_converter.h"
#include "shell/common/gin_helper/dictionary.h"

namespace gin {

namespace {

bool ParseAnchorPercentage(std::string anchor_str, int* percentage) {
  if (anchor_str.empty())
    return false;
  if (!base::StringToInt(anchor_str, percentage))
    return false;
  if (*percentage > 100)
    *percentage = 100;
  else if (*percentage < 0)
    *percentage = 0;
  return true;
}

void ConvertAnimationOptions(const gin_helper::Dictionary& params,
                             electron::AnimationOptions* options) {
  float duration = 1.0;
  if (params.Get("duration", &duration)) {
    options->duration = duration;
    options->animation = true;
  }

  std::string tfunction_name;
  if (params.Get("timingFunction", &tfunction_name)) {
    tfunction_name = base::ToLowerASCII(tfunction_name);
    base::TrimWhitespaceASCII(tfunction_name, base::TRIM_ALL, &tfunction_name);
    if (tfunction_name == "linear")
      options->timing_function = electron::TimingFunction::kLinear;
    else if (tfunction_name == "easein")
      options->timing_function = electron::TimingFunction::kEaseIn;
    else if (tfunction_name == "easeout")
      options->timing_function = electron::TimingFunction::kEaseOut;
    else if (tfunction_name == "easeineaseout")
      options->timing_function = electron::TimingFunction::kEaseInEaseOut;
    else if (tfunction_name == "default")
      options->timing_function = electron::TimingFunction::kDefault;
    options->animation = true;
  }

  float cx1 = 0.0, cy1 = 0.0, cx2 = 1.0, cy2 = 1.0;
  gin_helper::Dictionary timing_control_points;
  if (params.Get("timingControlPoints", &timing_control_points)) {
    if (timing_control_points.Get("x1", &cx1)) {
      options->cx1 = cx1;
      options->use_control_points = true;
    }
    if (timing_control_points.Get("y1", &cy1)) {
      options->cy1 = cy1;
      options->use_control_points = true;
    }
    if (timing_control_points.Get("x2", &cx2)) {
      options->cx2 = cx2;
      options->use_control_points = true;
    }
    if (timing_control_points.Get("y2", &cy2)) {
      options->cy2 = cy2;
      options->use_control_points = true;
    }
    options->animation = true;
  }
}

}  // namespace

bool Converter<electron::AnimationOptions>::FromV8(
    v8::Isolate* isolate,
    v8::Local<v8::Value> val,
    electron::AnimationOptions* options) {
  gin_helper::Dictionary params;
  if (!ConvertFromV8(isolate, val, &params))
    return false;

  *options = electron::AnimationOptions();

  ConvertAnimationOptions(params, options);

  return true;
}

bool Converter<electron::BoundsAnimationOptions>::FromV8(
    v8::Isolate* isolate,
    v8::Local<v8::Value> val,
    electron::BoundsAnimationOptions* options) {
  gin_helper::Dictionary params;
  if (!ConvertFromV8(isolate, val, &params))
    return false;

  *options = electron::BoundsAnimationOptions();

  ConvertAnimationOptions(params, options);

  gfx::Rect from_bounds;
  if (params.Get("fromBounds", &from_bounds)) {
    options->from_bounds = from_bounds;
    options->use_from_bounds = true;
  }

  return true;
}

bool Converter<electron::ScaleAnimationOptions>::FromV8(
    v8::Isolate* isolate,
    v8::Local<v8::Value> val,
    electron::ScaleAnimationOptions* options) {
  gin_helper::Dictionary params;
  if (!ConvertFromV8(isolate, val, &params))
    return false;

  *options = electron::ScaleAnimationOptions();

  gin_helper::Dictionary animation_params;
  if (params.Get("animation", &animation_params))
    ConvertAnimationOptions(animation_params, options);

  float scale_x = 1.0, scale_y = 1.0;
  if (params.Get("scaleX", &scale_x))
    options->scale_x = scale_x;
  if (params.Get("scaleY", &scale_y))
    options->scale_y = scale_y;
  bool adjust_frame = true;
  if (params.Get("adjustFrame", &adjust_frame))
    options->adjust_frame = adjust_frame;

  int percentage;
  std::string anchor_x;
  if (params.Get("anchorX", &anchor_x)) {
    anchor_x = base::ToLowerASCII(anchor_x);
    base::TrimWhitespaceASCII(anchor_x, base::TRIM_ALL, &anchor_x);
    if (anchor_x == "left") {
      options->anchor_x_pos = electron::AnchorXPos::kLeft;
    } else if (anchor_x == "center") {
      options->anchor_x_pos = electron::AnchorXPos::kCenter;
    } else if (anchor_x == "right") {
      options->anchor_x_pos = electron::AnchorXPos::kRight;
    } else if (ParseAnchorPercentage(anchor_x, &percentage)) {
      options->anchor_x_percentage = static_cast<float>(percentage);
      options->anchor_x_pos = electron::AnchorXPos::kPercentage;
    }
  }

  std::string anchor_y;
  if (params.Get("anchorY", &anchor_y)) {
    anchor_y = base::ToLowerASCII(anchor_y);
    base::TrimWhitespaceASCII(anchor_y, base::TRIM_ALL, &anchor_y);
    if (anchor_y == "top") {
      options->anchor_y_pos = electron::AnchorYPos::kTop;
    } else if (anchor_y == "center") {
      options->anchor_y_pos = electron::AnchorYPos::kCenter;
    } else if (anchor_y == "bottom") {
      options->anchor_y_pos = electron::AnchorYPos::kBottom;
    } else if (ParseAnchorPercentage(anchor_y, &percentage)) {
      options->anchor_y_percentage = static_cast<float>(percentage);
      options->anchor_y_pos = electron::AnchorYPos::kPercentage;
    }
  }

  return true;
}

}  // namespace gin
