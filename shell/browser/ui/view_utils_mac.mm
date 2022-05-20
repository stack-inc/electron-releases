#include "shell/browser/ui/view_utils.h"

#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>

#include <string>

#include "base/cxx17_backports.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "shell/common/gin_converters/gfx_converter.h"
#include "shell/common/gin_helper/dictionary.h"
#include "ui/gfx/geometry/rect.h"

namespace electron {

namespace {

const NSSize unitSize = {1.0, 1.0};

bool ParseAnchorPercentage(std::string anchor_str,
                           float diff_val,
                           float* diff_result) {
  if (anchor_str.empty())
    return false;
  int percentage = 0;
  if (!base::StringToInt(anchor_str, &percentage))
    return false;
  if (percentage > 100)
    percentage = 100;
  else if (percentage < 0)
    percentage = 0;
  *diff_result = diff_val * (static_cast<float>(percentage) / 100.0);
  return true;
}

}  // namespace

void SetBoundsForView(NSView* view,
                      const gfx::Rect& bounds,
                      const gin_helper::Dictionary& options) {
  bool animation = false;
  float duration = 1.0;
  std::string tfunction_name;
  float cx1 = 0.0, cy1 = 0.0, cx2 = 1.0, cy2 = 1.0;
  bool use_control_points = false;
  gfx::Rect from_bounds;
  if (options.Get("duration", &duration))
    animation = true;
  if (options.Get("timingFunction", &tfunction_name)) {
    animation = true;
    tfunction_name = base::ToLowerASCII(tfunction_name);
    base::TrimWhitespaceASCII(tfunction_name, base::TRIM_ALL, &tfunction_name);
  }
  gin_helper::Dictionary timing_control_points;
  if (options.Get("timingControlPoints", &timing_control_points)) {
    animation = true;
    use_control_points = true;
    timing_control_points.Get("x1", &cx1);
    timing_control_points.Get("y1", &cy1);
    timing_control_points.Get("x2", &cx2);
    timing_control_points.Get("y2", &cy2);
  }

  NSRect frame = bounds.ToCGRect();
  auto* superview = view.superview;
  if (superview && ![superview isFlipped]) {
    const auto superview_height = superview.frame.size.height;
    frame.origin.y = superview_height - bounds.y() - bounds.height();
  }

  if (!animation) {
    [view setFrame:frame];
    [view setNeedsDisplay:YES];
    // Calling setFrame manually does not trigger resizeSubviewsWithOldSize.
    [view resizeSubviewsWithOldSize:frame.size];
    return;
  }

  NSRect fromFrame = view.frame;
  if (options.Get("fromBounds", &from_bounds)) {
    if (superview && ![superview isFlipped]) {
      const auto superview_height = superview.frame.size.height;
      fromFrame =
          NSMakeRect(from_bounds.x(),
                     superview_height - from_bounds.y() - from_bounds.height(),
                     from_bounds.width(), from_bounds.height());
    } else {
      fromFrame = from_bounds.ToCGRect();
    }
  }

  CAMediaTimingFunction* timing_function = nil;
  if (!use_control_points) {
    CAMediaTimingFunctionName timing_function_name =
        kCAMediaTimingFunctionLinear;
    if (tfunction_name == "linear") {
      timing_function_name = kCAMediaTimingFunctionLinear;
    } else if (tfunction_name == "easein") {
      timing_function_name = kCAMediaTimingFunctionEaseIn;
    } else if (tfunction_name == "easeout") {
      timing_function_name = kCAMediaTimingFunctionEaseOut;
    } else if (tfunction_name == "easeineaseout") {
      timing_function_name = kCAMediaTimingFunctionEaseInEaseOut;
    } else if (tfunction_name == "default") {
      timing_function_name = kCAMediaTimingFunctionDefault;
    }
    timing_function =
        [CAMediaTimingFunction functionWithName:timing_function_name];
  } else {
    timing_function =
        [[CAMediaTimingFunction alloc] initWithControlPoints:cx1:cy1:cx2:cy2];
  }

  CABasicAnimation* positionAnimation =
      [CABasicAnimation animationWithKeyPath:@"position"];
  positionAnimation.duration = duration;
  positionAnimation.timingFunction = timing_function;
  positionAnimation.fromValue = @(fromFrame.origin);
  positionAnimation.toValue = @(frame.origin);
  if (fromFrame.size.width == frame.size.width &&
      fromFrame.size.height == frame.size.height) {
    [view.layer addAnimation:positionAnimation forKey:@"position"];
  } else {
    CABasicAnimation* sizeAnimation =
        [CABasicAnimation animationWithKeyPath:@"bounds.size"];
    sizeAnimation.duration = duration;
    sizeAnimation.timingFunction = timing_function;
    sizeAnimation.fromValue = @(fromFrame.size);
    sizeAnimation.toValue = @(frame.size);
    CAAnimationGroup* group = [CAAnimationGroup animation];
    group.duration = duration;
    group.timingFunction = timing_function;
    group.animations = @[ positionAnimation, sizeAnimation ];
    [view.layer addAnimation:group forKey:@"allMyAnimations"];
  }
  [view setFrame:frame];
}

void ResetScalingForView(NSView* view) {
  [view scaleUnitSquareToSize:[view convertSize:unitSize fromView:nil]];
}

void SetScaleForView(NSView* view, const gin_helper::Dictionary& options) {
  float scale_x = 1.0, scale_y = 1.0;
  options.Get("scaleX", &scale_x);
  options.Get("scaleY", &scale_y);
  bool adjust_frame = true;
  options.Get("adjustFrame", &adjust_frame);
  bool animation = false;
  float duration = 1.0;
  std::string tfunction;
  float cx1 = 0.0, cy1 = 0.0, cx2 = 1.0, cy2 = 1.0;
  bool use_control_points = false;
  gin_helper::Dictionary animation_options;
  if (options.Get("animation", &animation_options)) {
    animation = true;
    animation_options.Get("duration", &duration);
    if (animation_options.Get("timingFunction", &tfunction)) {
      tfunction = base::ToLowerASCII(tfunction);
      base::TrimWhitespaceASCII(tfunction, base::TRIM_ALL, &tfunction);
    }
    gin_helper::Dictionary timing_control_points;
    if (animation_options.Get("timingControlPoints", &timing_control_points)) {
      use_control_points = true;
      timing_control_points.Get("x1", &cx1);
      timing_control_points.Get("y1", &cy1);
      timing_control_points.Get("x2", &cx2);
      timing_control_points.Get("y2", &cy2);
    }
  }
  std::string anchor_x, anchor_y;
  if (options.Get("anchorX", &anchor_x)) {
    anchor_x = base::ToLowerASCII(anchor_x);
    base::TrimWhitespaceASCII(anchor_x, base::TRIM_ALL, &anchor_x);
  }
  if (options.Get("anchorY", &anchor_y)) {
    anchor_y = base::ToLowerASCII(anchor_y);
    base::TrimWhitespaceASCII(anchor_y, base::TRIM_ALL, &anchor_y);
  }

  NSSize current_scale = [view convertSize:unitSize toView:nil];
  NSRect current_frame = [view frame];

  NSSize new_scale;
  new_scale.width = scale_x / current_scale.width;
  new_scale.height = scale_y / current_scale.height;
  [view scaleUnitSquareToSize:new_scale];
  [view setNeedsDisplay:YES];

  if (adjust_frame) {
    // Set the frame to the scaled frame
    view.frame = NSMakeRect(
        current_frame.origin.x, current_frame.origin.y,
        (scale_x / current_scale.width) * current_frame.size.width,
        (scale_y / current_scale.height) * current_frame.size.height);
  }
  NSRect new_frame = [view frame];

  float width_diff = current_frame.size.width - new_frame.size.width;
  float height_diff = current_frame.size.height - new_frame.size.height;
  float offset_x = 0.0;
  float offset_y = 0.0;

  if (anchor_x == "right")
    offset_x = width_diff;
  else if (anchor_x == "center")
    offset_x = width_diff / 2;
  else if (anchor_x != "left")
    ParseAnchorPercentage(anchor_x, width_diff, &offset_x);

  auto* superview = view.superview;
  if (superview && ![superview isFlipped]) {
    if (anchor_y == "top")
      offset_y = height_diff;
    else if (anchor_y == "center")
      offset_y = height_diff / 2;
    else if (anchor_y != "bottom") {
      float diff_result = 0.0;
      if (ParseAnchorPercentage(anchor_y, height_diff, &diff_result))
        offset_y = height_diff - diff_result;
      else
        offset_y = height_diff;
    }
  } else {
    if (anchor_y == "bottom")
      offset_y = height_diff;
    else if (anchor_y == "center")
      offset_y = height_diff / 2;
    else if (anchor_y != "top")
      ParseAnchorPercentage(anchor_y, height_diff, &offset_y);
  }

  if (animation) {
    CAMediaTimingFunction* timing_function_scale = nil;
    CAMediaTimingFunction* timing_function_tran = nil;
    if (!use_control_points) {
      CAMediaTimingFunctionName timing_function_name =
          kCAMediaTimingFunctionLinear;
      if (tfunction == "linear") {
        timing_function_name = kCAMediaTimingFunctionLinear;
      } else if (tfunction == "easein") {
        timing_function_name = kCAMediaTimingFunctionEaseIn;
      } else if (tfunction == "easeout") {
        timing_function_name = kCAMediaTimingFunctionEaseOut;
      } else if (tfunction == "easeineaseout") {
        timing_function_name = kCAMediaTimingFunctionEaseInEaseOut;
      } else if (tfunction == "default") {
        timing_function_name = kCAMediaTimingFunctionDefault;
      }
      timing_function_scale =
          [CAMediaTimingFunction functionWithName:timing_function_name];
      if (offset_x != 0.0 || offset_y != 0.0)
        timing_function_tran =
            [CAMediaTimingFunction functionWithName:timing_function_name];
    } else {
      timing_function_scale =
          [[CAMediaTimingFunction alloc] initWithControlPoints:cx1:cy1:cx2:cy2];
      if (offset_x != 0.0 || offset_y != 0.0)
        timing_function_tran =
            [[CAMediaTimingFunction alloc] initWithControlPoints:
                                                             cx1:cy1:cx2:cy2];
    }

    // Create the scale animation
    CABasicAnimation* animation = [CABasicAnimation animation];
    animation.duration = duration;
    animation.timingFunction = timing_function_scale;
    animation.fromValue = @(
        CATransform3DMakeScale(current_scale.width, current_scale.height, 1.0));
    animation.toValue = @(CATransform3DMakeScale(scale_x, scale_y, 1.0));
    // Trigger the scale animation
    [view.layer addAnimation:animation forKey:@"transform"];

    if (offset_x != 0.0 || offset_y != 0.0) {
      // Add a simultaneous translation animation to keep the
      // view center static during the zoom animation
      [NSAnimationContext
          runAnimationGroup:^(NSAnimationContext* context) {
            // Match the configuration of the scale animation
            context.duration = duration;
            context.timingFunction = timing_function_tran;
            view.animator.frame = CGRectOffset(view.frame, offset_x, offset_y);
          }
          completionHandler:^{
          }];
    }
  } else {
    if (offset_x != 0.0 || offset_y != 0.0)
      view.frame = CGRectOffset(view.frame, offset_x, offset_y);
  }
}

float GetScaleXForView(NSView* view) {
  NSSize size = [view convertSize:unitSize toView:nil];
  return size.width;
}

float GetScaleYForView(NSView* view) {
  NSSize size = [view convertSize:unitSize toView:nil];
  return size.height;
}

void SetOpacityForView(NSView* view,
                       const double opacity,
                       const gin_helper::Dictionary& options) {
  bool animation = false;
  float duration = 1.0;
  std::string tfunction_name;
  float cx1 = 0.0, cy1 = 0.0, cx2 = 1.0, cy2 = 1.0;
  bool use_control_points = false;
  if (options.Get("duration", &duration))
    animation = true;
  if (options.Get("timingFunction", &tfunction_name)) {
    animation = true;
    tfunction_name = base::ToLowerASCII(tfunction_name);
    base::TrimWhitespaceASCII(tfunction_name, base::TRIM_ALL, &tfunction_name);
  }
  gin_helper::Dictionary timing_control_points;
  if (options.Get("timingControlPoints", &timing_control_points)) {
    animation = true;
    use_control_points = true;
    timing_control_points.Get("x1", &cx1);
    timing_control_points.Get("y1", &cy1);
    timing_control_points.Get("x2", &cx2);
    timing_control_points.Get("y2", &cy2);
  }

  const double bounded_opacity = base::clamp(opacity, 0.0, 1.0);
  if (!animation) {
    [view setAlphaValue:bounded_opacity];
    return;
  }

  CAMediaTimingFunction* timing_function = nil;
  if (!use_control_points) {
    CAMediaTimingFunctionName timing_function_name =
        kCAMediaTimingFunctionLinear;
    if (tfunction_name == "linear") {
      timing_function_name = kCAMediaTimingFunctionLinear;
    } else if (tfunction_name == "easein") {
      timing_function_name = kCAMediaTimingFunctionEaseIn;
    } else if (tfunction_name == "easeout") {
      timing_function_name = kCAMediaTimingFunctionEaseOut;
    } else if (tfunction_name == "easeineaseout") {
      timing_function_name = kCAMediaTimingFunctionEaseInEaseOut;
    } else if (tfunction_name == "default") {
      timing_function_name = kCAMediaTimingFunctionDefault;
    }
    timing_function =
        [CAMediaTimingFunction functionWithName:timing_function_name];
  } else {
    timing_function =
        [[CAMediaTimingFunction alloc] initWithControlPoints:cx1:cy1:cx2:cy2];
  }

  [NSAnimationContext
      runAnimationGroup:^(NSAnimationContext* context) {
        context.duration = duration;
        context.timingFunction = timing_function;
        // Trigger the animation
        view.animator.alphaValue = bounded_opacity;
      }
      completionHandler:^{
      }];
}

double GetOpacityForView(NSView* view) {
  return [view alphaValue];
}

}  // namespace electron
