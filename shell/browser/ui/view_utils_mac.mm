#include "shell/browser/ui/view_utils.h"

#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>

#include "base/cxx17_backports.h"

namespace electron {

namespace {

const NSSize unitSize = {1.0, 1.0};

}  // namespace

void SetBoundsForView(NATIVEVIEW view,
                      const gfx::Rect& bounds,
                      const BoundsAnimationOptions& options,
                      NativeView* native_view /*= nullptr*/) {
  NSRect frame = bounds.ToCGRect();
  auto* superview = view.superview;
  if (superview && ![superview isFlipped]) {
    const auto superview_height = superview.frame.size.height;
    frame.origin.y = superview_height - bounds.y() - bounds.height();
  }

  if (!options.animation) {
    [view setFrame:frame];
    [view setNeedsDisplay:YES];
    // Calling setFrame manually does not trigger resizeSubviewsWithOldSize.
    [view resizeSubviewsWithOldSize:frame.size];
    return;
  }

  NSRect fromFrame = view.frame;
  if (options.use_from_bounds) {
    if (superview && ![superview isFlipped]) {
      const auto superview_height = superview.frame.size.height;
      fromFrame =
          NSMakeRect(options.from_bounds.x(),
                     superview_height - options.from_bounds.y() -
                         options.from_bounds.height(),
                     options.from_bounds.width(), options.from_bounds.height());
    } else {
      fromFrame = options.from_bounds.ToCGRect();
    }
  }

  CAMediaTimingFunction* timing_function = nil;
  if (!options.use_control_points) {
    CAMediaTimingFunctionName timing_function_name =
        kCAMediaTimingFunctionLinear;
    if (options.timing_function == TimingFunction::kLinear)
      timing_function_name = kCAMediaTimingFunctionLinear;
    else if (options.timing_function == TimingFunction::kEaseIn)
      timing_function_name = kCAMediaTimingFunctionEaseIn;
    else if (options.timing_function == TimingFunction::kEaseOut)
      timing_function_name = kCAMediaTimingFunctionEaseOut;
    else if (options.timing_function == TimingFunction::kEaseInEaseOut)
      timing_function_name = kCAMediaTimingFunctionEaseInEaseOut;
    else if (options.timing_function == TimingFunction::kDefault)
      timing_function_name = kCAMediaTimingFunctionDefault;
    timing_function =
        [CAMediaTimingFunction functionWithName:timing_function_name];
  } else {
    timing_function =
        [[CAMediaTimingFunction alloc] initWithControlPoints:options.cx1:options.cy1:options.cx2:options.cy2];
  }

  CABasicAnimation* positionAnimation =
      [CABasicAnimation animationWithKeyPath:@"position"];
  positionAnimation.duration = options.duration;
  positionAnimation.timingFunction = timing_function;
  positionAnimation.fromValue = @(fromFrame.origin);
  positionAnimation.toValue = @(frame.origin);
  if (fromFrame.size.width == frame.size.width &&
      fromFrame.size.height == frame.size.height) {
    [view.layer addAnimation:positionAnimation forKey:@"position"];
  } else {
    CABasicAnimation* sizeAnimation =
        [CABasicAnimation animationWithKeyPath:@"bounds.size"];
    sizeAnimation.duration = options.duration;
    sizeAnimation.timingFunction = timing_function;
    sizeAnimation.fromValue = @(fromFrame.size);
    sizeAnimation.toValue = @(frame.size);
    CAAnimationGroup* group = [CAAnimationGroup animation];
    group.duration = options.duration;
    group.timingFunction = timing_function;
    group.animations = @[ positionAnimation, sizeAnimation ];
    [view.layer addAnimation:group forKey:@"allMyAnimations"];
  }
  [view setFrame:frame];
}

void ResetScalingForView(NATIVEVIEW view) {
  [view scaleUnitSquareToSize:[view convertSize:unitSize fromView:nil]];
}

void SetScaleForView(NATIVEVIEW view, const ScaleAnimationOptions& options) {
  NSSize current_scale = [view convertSize:unitSize toView:nil];
  NSRect current_frame = [view frame];

  NSSize new_scale;
  new_scale.width = options.scale_x / current_scale.width;
  new_scale.height = options.scale_y / current_scale.height;
  [view scaleUnitSquareToSize:new_scale];
  [view setNeedsDisplay:YES];

  if (options.adjust_frame) {
    // Set the frame to the scaled frame
    view.frame = NSMakeRect(
        current_frame.origin.x, current_frame.origin.y,
        (options.scale_x / current_scale.width) * current_frame.size.width,
        (options.scale_y / current_scale.height) * current_frame.size.height);
  }
  NSRect new_frame = [view frame];

  float width_diff = current_frame.size.width - new_frame.size.width;
  float height_diff = current_frame.size.height - new_frame.size.height;
  float offset_x = 0.0;
  float offset_y = 0.0;

  if (options.anchor_x_pos == AnchorXPos::kRight)
    offset_x = width_diff;
  else if (options.anchor_x_pos == AnchorXPos::kCenter)
    offset_x = width_diff / 2;
  else if (options.anchor_x_pos == AnchorXPos::kPercentage)
    offset_x = width_diff * (options.anchor_x_percentage / 100.0);

  auto* superview = view.superview;
  if (superview && ![superview isFlipped]) {
    if (options.anchor_y_pos == AnchorYPos::kTop)
      offset_y = height_diff;
    else if (options.anchor_y_pos == AnchorYPos::kCenter)
      offset_y = height_diff / 2;
    else if (options.anchor_y_pos == AnchorYPos::kPercentage)
      offset_y =
          height_diff - height_diff * (options.anchor_y_percentage / 100.0);
  } else {
    if (options.anchor_y_pos == AnchorYPos::kBottom)
      offset_y = height_diff;
    else if (options.anchor_y_pos == AnchorYPos::kCenter)
      offset_y = height_diff / 2;
    else if (options.anchor_y_pos == AnchorYPos::kPercentage)
      offset_y = height_diff * (options.anchor_y_percentage / 100.0);
  }

  if (!options.animation) {
    if (offset_x != 0.0 || offset_y != 0.0)
      view.frame = CGRectOffset(view.frame, offset_x, offset_y);
    return;
  }

  CAMediaTimingFunction* timing_function_scale = nil;
  CAMediaTimingFunction* timing_function_tran = nil;
  if (!options.use_control_points) {
    CAMediaTimingFunctionName timing_function_name =
        kCAMediaTimingFunctionLinear;
    if (options.timing_function == TimingFunction::kLinear)
      timing_function_name = kCAMediaTimingFunctionLinear;
    else if (options.timing_function == TimingFunction::kEaseIn)
      timing_function_name = kCAMediaTimingFunctionEaseIn;
    else if (options.timing_function == TimingFunction::kEaseOut)
      timing_function_name = kCAMediaTimingFunctionEaseOut;
    else if (options.timing_function == TimingFunction::kEaseInEaseOut)
      timing_function_name = kCAMediaTimingFunctionEaseInEaseOut;
    else if (options.timing_function == TimingFunction::kDefault)
      timing_function_name = kCAMediaTimingFunctionDefault;
    timing_function_scale =
        [CAMediaTimingFunction functionWithName:timing_function_name];
    if (offset_x != 0.0 || offset_y != 0.0) {
      timing_function_tran =
          [CAMediaTimingFunction functionWithName:timing_function_name];
    }
  } else {
    timing_function_scale =
          [[CAMediaTimingFunction alloc] initWithControlPoints:options.cx1:options.cy1:options.cx2:options.cy2];
    if (offset_x != 0.0 || offset_y != 0.0) {
      timing_function_tran =
            [[CAMediaTimingFunction alloc] initWithControlPoints:
                                                             options.cx1:options.cy1:options.cx2:options.cy2];
    }
  }

  // Create the scale animation
  CABasicAnimation* animation = [CABasicAnimation animation];
  animation.duration = options.duration;
  animation.timingFunction = timing_function_scale;
  animation.fromValue =
      @(CATransform3DMakeScale(current_scale.width, current_scale.height, 1.0));
  animation.toValue =
      @(CATransform3DMakeScale(options.scale_x, options.scale_y, 1.0));
  // Trigger the scale animation
  [view.layer addAnimation:animation forKey:@"transform"];

  if (offset_x != 0.0 || offset_y != 0.0) {
    // Add a simultaneous translation animation
    [NSAnimationContext
        runAnimationGroup:^(NSAnimationContext* context) {
          // Match the configuration of the scale animation
          context.duration = options.duration;
          context.timingFunction = timing_function_tran;
          view.animator.frame = CGRectOffset(view.frame, offset_x, offset_y);
        }
        completionHandler:^{
        }];
  }
}

float GetScaleXForView(NATIVEVIEW view) {
  NSSize size = [view convertSize:unitSize toView:nil];
  return size.width;
}

float GetScaleYForView(NATIVEVIEW view) {
  NSSize size = [view convertSize:unitSize toView:nil];
  return size.height;
}

void SetOpacityForView(NATIVEVIEW view,
                       const double opacity,
                       const AnimationOptions& options) {
  const double bounded_opacity = base::clamp(opacity, 0.0, 1.0);
  if (!options.animation) {
    [view setAlphaValue:bounded_opacity];
    return;
  }

  CAMediaTimingFunction* timing_function = nil;
  if (!options.use_control_points) {
    CAMediaTimingFunctionName timing_function_name =
        kCAMediaTimingFunctionLinear;
    if (options.timing_function == TimingFunction::kLinear)
      timing_function_name = kCAMediaTimingFunctionLinear;
    else if (options.timing_function == TimingFunction::kEaseIn)
      timing_function_name = kCAMediaTimingFunctionEaseIn;
    else if (options.timing_function == TimingFunction::kEaseOut)
      timing_function_name = kCAMediaTimingFunctionEaseOut;
    else if (options.timing_function == TimingFunction::kEaseInEaseOut)
      timing_function_name = kCAMediaTimingFunctionEaseInEaseOut;
    else if (options.timing_function == TimingFunction::kDefault)
      timing_function_name = kCAMediaTimingFunctionDefault;
    timing_function =
        [CAMediaTimingFunction functionWithName:timing_function_name];
  } else {
    timing_function =
        [[CAMediaTimingFunction alloc] initWithControlPoints:options.cx1:options.cy1:options.cx2:options.cy2];
  }

  [NSAnimationContext
      runAnimationGroup:^(NSAnimationContext* context) {
        context.duration = options.duration;
        context.timingFunction = timing_function;
        // Trigger the animation
        view.animator.alphaValue = bounded_opacity;
      }
      completionHandler:^{
      }];
}

double GetOpacityForView(NATIVEVIEW view) {
  return [view alphaValue];
}

}  // namespace electron
