// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "electron/shell/browser/ui/views/stack_elastic_scroll/stack_elastic_scroll_animator.h"

#include <cmath>

#include "ui/gfx/animation/animation.h"

#include "base/logging.h"

namespace electron {
namespace {

const int kDefaultFrameRate = 60;
const double kMinimumDelta = 6.0f;
const double kDeltaFactor = 12.0f;

base::TimeDelta CalculateInterval(int frame_rate) {
  int timer_interval = 1000000 / frame_rate;
  if (timer_interval < 10000)
    timer_interval = 10000;
  return base::Microseconds(timer_interval);
}

}  // namespace

class StackElasticScrollAnimator::ContinuousAnimation : public gfx::Animation {
 public:
  ContinuousAnimation(gfx::AnimationDelegate* delegate)
      : Animation(CalculateInterval(kDefaultFrameRate)) {
    set_delegate(delegate);
  }
  ~ContinuousAnimation() override = default;

  double GetCurrentValue() const override {
    return delta_.InMillisecondsF() /  base::Time::kMillisecondsPerSecond;
  }

 protected:
  void AnimationStarted() override {
    last_time_ = base::TimeTicks::Now();
  }

  void Step(base::TimeTicks time_now) override {
    delta_ = time_now - last_time_;
    last_time_ = time_now;

    if (delegate())
      delegate()->AnimationProgressed(this);
  }

 private:
  base::TimeTicks last_time_;
  base::TimeDelta delta_;
};

StackElasticScrollAnimator::StackElasticScrollAnimator(
    views::ScrollBar* scroll_bar)
        : scroll_bar_(scroll_bar),
          animation_(nullptr),
          dest_offset_x_(0),
          dest_offset_y_(0),
          current_offset_x_(0),
          current_offset_y_(0) {
}
StackElasticScrollAnimator::~StackElasticScrollAnimator() {
  Stop();
}

void StackElasticScrollAnimator::Start(int offset_x, int offset_y) {
  if (!animation_) {
    current_offset_x_ = 0;
    current_offset_y_ = 0;
    dest_offset_x_ = 0;
    dest_offset_y_ = 0;
    animation_ = new ContinuousAnimation(this);
    animation_->Start();
  }

  dest_offset_x_ += offset_x;
  dest_offset_y_ += offset_y;
}

void StackElasticScrollAnimator::Stop() {
  if (animation_) {
    animation_->Stop();
    delete animation_;
    animation_ = nullptr;
  }
}

void StackElasticScrollAnimator::AnimationProgressed(
    const gfx::Animation* animation) {
  double delta_x = dest_offset_x_ - current_offset_x_;
  double delta_y = dest_offset_y_ - current_offset_y_;

  delta_x *= (animation->GetCurrentValue() * kDeltaFactor);
  delta_y *= (animation->GetCurrentValue() * kDeltaFactor);

  current_offset_x_ += delta_x;
  current_offset_y_ += delta_y;

  if (std::fabs(current_offset_x_) >= std::fabs(dest_offset_x_)) {
    delta_x -= (current_offset_x_ - dest_offset_x_);
    current_offset_x_ = dest_offset_x_;
  }

  if (std::fabs(current_offset_y_) >= std::fabs(dest_offset_y_)) {
    delta_y -= (current_offset_y_ - dest_offset_y_);
    current_offset_y_ = dest_offset_y_;
  }

  scroll_bar_->OnScroll(delta_x, delta_y);
  scroll_bar_->InvalidateLayout();
  if (std::fabs(delta_x) < kMinimumDelta &&
      std::fabs(delta_y) < kMinimumDelta) {
    Stop();
  }
}

}  // namespace electron
